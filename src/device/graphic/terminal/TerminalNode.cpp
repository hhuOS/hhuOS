/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

namespace Device::Graphic {

TerminalNode::TerminalNode(const Util::Memory::String &name, Util::Graphic::Terminal *terminal) : Filesystem::Memory::StreamNode(name, terminal, terminal), terminal(terminal){}

bool TerminalNode::control(uint32_t request, const Util::Data::Array<uint32_t> &parameters) {
    switch (request) {
        case Util::Graphic::Terminal::Command::SET_ECHO:
            terminal->setEcho(parameters[0]);
            return true;
        case Util::Graphic::Terminal::Command::SET_LINE_AGGREGATION:
            terminal->setLineAggregation(parameters[0]);
            return true;
        case Util::Graphic::Terminal::Command::SET_CURSOR:
            terminal->setCursor(parameters[0]);
            return true;
        case Util::Graphic::Terminal::Command::SET_ANSI_PARSING:
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
        default:
            return false;
    }
}

}