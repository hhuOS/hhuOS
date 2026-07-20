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

#include "TerminalNode.h"

#include "lib/util/graphic/Terminal.h"
#include "lib/util/io/key/layout/UsLayout.h"
#include "lib/util/base/Panic.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/collection/Array.h"

namespace Util {
namespace Io {
class KeyboardLayout;
}  // namespace Io
}  // namespace Util

namespace Device::Graphic {

TerminalNode::TerminalNode(const Util::String &name, Util::Graphic::Terminal *terminal) : StreamNode(name, terminal, terminal), terminal(terminal) {}

int64_t TerminalNode::control(const uint32_t request, const uint32_t arg0, const uint32_t, const uint32_t) {
    switch (request) {
        case Util::Graphic::Terminal::Command::SET_ECHO:
            terminal->setEcho(arg0);
            return 0;
        case Util::Graphic::Terminal::Command::SET_LINE_AGGREGATION:
            terminal->setLineAggregation(arg0);
            return 0;
        case Util::Graphic::Terminal::Command::SET_CURSOR:
            terminal->setCursorEnabled(arg0);
            return 0;
        case Util::Graphic::Terminal::Command::SET_ANSI_PARSING:
            terminal->setAnsiParsing(arg0);
            return 0;
        case Util::Graphic::Terminal::Command::ENABLE_RAW_MODE:
            terminal->setEcho(false);
            terminal->setAnsiParsing(false);
            terminal->setLineAggregation(false);
            terminal->setKeyboardScancodes(false);
            return 0;
        case Util::Graphic::Terminal::Command::ENABLE_CANONICAL_MODE:
            terminal->setEcho(true);
            terminal->setAnsiParsing(true);
            terminal->setLineAggregation(true);
            terminal->setKeyboardScancodes(false);
            return 0;
        case Util::Graphic::Terminal::Command::ENABLE_KEYBOARD_SCANCODES:
            terminal->setEcho(false);
            terminal->setLineAggregation(false);
            terminal->setKeyboardScancodes(true);
            return 0;
        case Util::Graphic::Terminal::Command::SET_KEYBOARD_LAYOUT: {
            const auto layoutName = Util::String(reinterpret_cast<const char *>(arg0)).toLowerCase();

            if (layoutName == "de") {
                terminal->setKeyboardLayout(Util::Io::DeLayout());
            } else if (layoutName == "us") {
                terminal->setKeyboardLayout(Util::Io::UsLayout());
            } else {
                return false;
            }

            return 0;
        }
        default:
            return -1;
    }
}

bool TerminalNode::isReadyToRead() {
    return terminal->isReadyToRead();
}

}