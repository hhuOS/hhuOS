/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "TerminalNode.h"

#include "lib/util/graphic/Terminal.h"
#include "lib/util/io/key/layout/UsLayout.h"
#include "lib/util/base/Exception.h"
#include "lib/util/io/key/layout/DeLayout.h"
#include "lib/util/collection/Array.h"

namespace Util {
namespace Io {
class KeyboardLayout;
}  // namespace Io
}  // namespace Util

namespace Device::Graphic {

TerminalNode::TerminalNode(const Util::String &name, Util::Graphic::Terminal *terminal) : Filesystem::Memory::StreamNode(name, terminal, terminal), terminal(terminal) {}

bool TerminalNode::control(uint32_t request, const Util::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Graphic::Terminal::Command::SET_ECHO:
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Terminal: Missing parameter for configuring echo!");
            }

            terminal->setEcho(parameters[0]);
            return true;
        case Util::Graphic::Terminal::Command::SET_LINE_AGGREGATION:
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Terminal: Missing parameter for configuring line aggregation!");
            }

            terminal->setLineAggregation(parameters[0]);
            return true;
        case Util::Graphic::Terminal::Command::SET_CURSOR:
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Terminal: Missing parameter for configuring cursor!");
            }

            terminal->setCursor(parameters[0]);
            return true;
        case Util::Graphic::Terminal::Command::SET_ANSI_PARSING:
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Terminal: Missing parameter for configuring ANSI parsing!");
            }

            terminal->setAnsiParsing(parameters[0]);
            return true;
        case Util::Graphic::Terminal::Command::ENABLE_RAW_MODE:
            terminal->setEcho(false);
            terminal->setAnsiParsing(false);
            terminal->setLineAggregation(false);
            terminal->setKeyboardScancodes(false);
            return true;
        case Util::Graphic::Terminal::Command::ENABLE_CANONICAL_MODE:
            terminal->setEcho(true);
            terminal->setAnsiParsing(true);
            terminal->setLineAggregation(true);
            terminal->setKeyboardScancodes(false);
            return true;
        case Util::Graphic::Terminal::Command::ENABLE_KEYBOARD_SCANCODES:
            terminal->setEcho(false);
            terminal->setAnsiParsing(false);
            terminal->setLineAggregation(false);
            terminal->setKeyboardScancodes(true);
            return true;
        case Util::Graphic::Terminal::Command::SET_KEYBOARD_LAYOUT: {
            if (parameters.length() < 1) {
                Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Terminal: Missing parameter for configuring keyboard layout!");
            }

            const auto layoutName = Util::String(reinterpret_cast<const char *>(parameters[0])).toLowerCase();
            Util::Io::KeyboardLayout *layout = nullptr;

            if (layoutName == "de") {
                layout = new Util::Io::DeLayout();
            } else if (layoutName == "us") {
                layout = new Util::Io::UsLayout();
            } else {
                return false;
            };

            terminal->setKeyboardLayout(layout);
            return true;
        }
        default:
            Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Terminal: Invalid control request!");
    }
}

bool TerminalNode::isReadyToRead() {
    return terminal->isReadyToRead();
}

}