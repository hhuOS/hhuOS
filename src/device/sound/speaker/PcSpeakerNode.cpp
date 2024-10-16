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

#include "PcSpeaker.h"
#include "PcSpeakerNode.h"

namespace Device::Sound {

PcSpeakerNode::PcSpeakerNode(const Util::String &name) : StringNode(name) {}

Util::String PcSpeakerNode::getString() {
    return buffer;
}

Util::Io::File::Type PcSpeakerNode::getType() {
    return Util::Io::File::CHARACTER;
}

uint64_t PcSpeakerNode::writeData(const uint8_t *sourceBuffer, [[maybe_unused]] uint64_t pos, uint64_t numBytes) {
    auto data = Util::String(sourceBuffer, numBytes);
    currentFrequency = Util::String::parseInt(data);
    buffer = Util::String::format("%u\n", currentFrequency);

    PcSpeaker::play(currentFrequency);
    return numBytes;
}

}