/*
* Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "KeyboardDemo.h"

#include "util/base/System.h"
#include "util/graphic/Ansi.h"
#include "util/io/key/KeyDecoder.h"
#include "util/io/key/layout/DeLayout.h"

void keyboardDemo() {
    auto keyDecoder = Util::Io::KeyDecoder(Util::Io::DeLayout());
    Util::Graphic::Ansi::prepareGraphicalApplication(true);
    Util::Graphic::Ansi::enableCursor();

    Util::System::out << "Press keys to see their scancode, ASCII code and modifiers." <<
        " Press ESC to exit." << Util::Io::PrintStream::lnFlush;

    auto c = Util::System::in.read();
    while (c > 0) {
        if (keyDecoder.parseScancode(static_cast<uint8_t>(c))) {
            auto key = keyDecoder.getKeyEvent();

            if (key.getScancode() == Util::Io::KeyEvent::ESC) {
                break;
            }

            Util::ArrayList<Util::String> modifiers;
            if (key.getShift()) {
                modifiers.add("Shift");
            }
            if (key.getAltLeft()) {
                modifiers.add("AltLeft");
            }
            if (key.getAltRight()) {
                modifiers.add("AltRight");
            }
            if (key.getCtrlLeft()) {
                modifiers.add("CtrlLeft");
            }
            if (key.getCtrlRight()) {
                modifiers.add("CtrlRight");
            }

            Util::System::out << Util::Io::PrintStream::hex << "Key " << (key.isPressed() ? "pressed:" : "released:") <<
                " [scancode = 0x" << key.getScancode() <<
                ", ascii = '" << key.getAscii() << "'" <<
                ", modifiers = (";

            for (size_t i = 0; i < modifiers.size(); i++) {
                Util::System::out << modifiers.get(i);
                if (i < modifiers.size() - 1) {
                    Util::System::out << " | ";
                }
            }

            Util::System::out << ")]" << Util::Io::PrintStream::lnFlush;
        }

        c = Util::System::in.read();
    }
}
