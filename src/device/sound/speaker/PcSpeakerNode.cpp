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

#include "PcSpeaker.h"
#include "PcSpeakerNode.h"

namespace Device::Sound {

PcSpeakerNode::PcSpeakerNode(const Util::Memory::String &name) : StringNode(name) {}

Util::Memory::String PcSpeakerNode::getString() {
    return buffer;
}

Util::File::Type PcSpeakerNode::getFileType() {
    return Util::File::CHARACTER;
}

uint64_t PcSpeakerNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    auto data = Util::Memory::String(sourceBuffer, numBytes);
    currentFrequency = Util::Memory::String::parseInt(data);
    buffer = Util::Memory::String::format("%u\n", currentFrequency);

    PcSpeaker::play(currentFrequency);
    return numBytes;
}

}