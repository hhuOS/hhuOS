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

#include "MemoryStatusNode.h"
#include "lib/util/memory/Address.h"
#include "kernel/system/System.h"

namespace Kernel {

MemoryStatusNode::MemoryStatusNode(const Util::Memory::String &name) : Filesystem::Memory::MemoryNode(name) {}

uint64_t MemoryStatusNode::getLength() {
    refreshBuffer();
    return memoryStatusBuffer.length();
}

uint64_t MemoryStatusNode::readData(uint8_t *targetBuffer, uint64_t pos, uint64_t numBytes) {
    refreshBuffer();

    if (pos >= memoryStatusBuffer.length()) {
        return 0;
    }

    if (pos + numBytes > memoryStatusBuffer.length()) {
        numBytes = (memoryStatusBuffer.length() - pos);
    }

    auto sourceAddress = Util::Memory::Address<uint32_t>(static_cast<const char *>(memoryStatusBuffer)).add(pos);
    auto targetAddress = Util::Memory::Address<uint32_t>(targetBuffer);
    targetAddress.copyRange(sourceAddress, numBytes);

    return numBytes;
}

void MemoryStatusNode::refreshBuffer() {
    auto memoryStatus = Kernel::System::getService<Kernel::MemoryService>().getMemoryStatus();
    memoryStatusBuffer = "Physical:      " + formatMemory(memoryStatus.freePhysicalMemory) + " / " + formatMemory(memoryStatus.totalPhysicalMemory) + "\n"
            + "Lower:         " + formatMemory(memoryStatus.freeLowerMemory) + " / " + formatMemory(memoryStatus.totalLowerMemory) + "\n"
            + "Kernel:        " + formatMemory(memoryStatus.freeKernelHeapMemory) + " / " + formatMemory(memoryStatus.totalKernelHeapMemory) + "\n"
            + "Paging Area:   " + formatMemory(memoryStatus.freePagingAreaMemory) + " / " + formatMemory(memoryStatus.totalPagingAreaMemory) + "\n";
}

Util::Memory::String MemoryStatusNode::formatMemory(uint32_t value) {
    uint32_t result = value / 1024 / 1024;
    uint32_t rest = value - (result * 1024 * 1024);
    uint32_t comma = (rest * 1000) / 1024 / 1024;

    return Util::Memory::String::format("%u.%u MiB", result, comma);
}

}