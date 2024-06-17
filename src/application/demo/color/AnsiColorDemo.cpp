/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#include <cstdint>

#include "lib/util/graphic/Ansi.h"
#include "lib/util/graphic/Color.h"
#include "lib/util/io/stream/PrintStream.h"
#include "lib/util/base/System.h"

void ansiColorDemo() {
    Util::System::out << Util::Graphic::Ansi::BACKGROUND_DEFAULT << "4-bit colors:" << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;

    for (uint32_t i = 0; i < 16; i++) {
        Util::System::out << Util::Graphic::Ansi::background8BitColor(i) << " ";
    }

    Util::System::out << Util::Graphic::Ansi::BACKGROUND_DEFAULT << Util::Io::PrintStream::endl << Util::Io::PrintStream::endl << "8-bit colors:";

    for (uint32_t i = 0; i < 216; i++) {
        if (i % 36 == 0) {
            Util::System::out << Util::Graphic::Ansi::BACKGROUND_DEFAULT << Util::Io::PrintStream::endl;
        }
        Util::System::out << Util::Graphic::Ansi::background8BitColor(i + 16) << " ";
    }

    Util::System::out << Util::Graphic::Ansi::BACKGROUND_DEFAULT << Util::Io::PrintStream::endl << Util::Io::PrintStream::endl << "Grayscale colors:" << Util::Io::PrintStream::endl;

    for (uint32_t i = 232; i < 256; i++) {
        Util::System::out << Util::Graphic::Ansi::background8BitColor(i) << " ";
    }

    Util::System::out << Util::Graphic::Ansi::BACKGROUND_DEFAULT << Util::Io::PrintStream::endl << Util::Io::PrintStream::endl << "24-bit colors:" << Util::Io::PrintStream::endl;

    for (uint32_t i = 0; i < 8; i++) {
        for (uint32_t j = 0; j < 8; j++) {
            for (uint32_t k = 0; k < 8; k++) {
                Util::System::out << Util::Graphic::Ansi::background24BitColor(Util::Graphic::Color(i * 32, j * 32, k * 32)) << " ";
            }
        }
        Util::System::out << Util::Graphic::Ansi::BACKGROUND_DEFAULT << Util::Io::PrintStream::endl;
    }

    Util::System::out << Util::Io::PrintStream::flush;
}
