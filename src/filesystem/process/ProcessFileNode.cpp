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

#include "ProcessFileNode.h"
#include "lib/util/memory/Address.h"

namespace Filesystem::Process {

ProcessFileNode::ProcessFileNode(const Util::Memory::String &name, const Util::Memory::String &content) : name(name), buffer(content + "\n") {}

Util::Memory::String ProcessFileNode::getName() {
    return name;
}

Util::File::Type ProcessFileNode::getFileType() {
    return Util::File::REGULAR;
}

uint64_t ProcessFileNode::getLength() {
    return buffer.length();
}

Util::Data::Array<Util::Memory::String> ProcessFileNode::getChildren() {
    return Util::Data::Array<Util::Memory::String>(0);
}

uint64_t ProcessFileNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    if (pos >= buffer.length()) {
        return 0;
    }

    if (pos + numBytes > buffer.length()) {
        numBytes = (buffer.length() - pos);
    }

    auto sourceAddress = Util::Memory::Address<uint32_t>(static_cast<const char*>(buffer)).add(pos);
    auto targetAddress = Util::Memory::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(sourceAddress, numBytes);

    return numBytes;
}

uint64_t ProcessFileNode::writeData(const uint8_t *sourceBuffer, uint64_t pos, uint64_t numBytes) {
    return 0;
}

}