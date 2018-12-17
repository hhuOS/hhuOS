/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#include <kernel/memory/MemLayout.h>
#include "IsaDmaMemoryManager.h"

IsaDmaMemoryManager::IsaDmaMemoryManager() : BitmapMemoryManager(ISA_DMA_BUF_SIZE, true) {
    BitmapMemoryManager::init(ISA_DMA_START_ADDRESS, ISA_DMA_END_ADDRESS, false);

    // Reserve every block > 512 KB.
    freeBitmap[0] = 0x00ffffff;
}

void IsaDmaMemoryManager::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    // Do nothing. The IsaDmaMemoryManager will always be initialized by the kernel and has hardcoded values.
}

String IsaDmaMemoryManager::getTypeName() {
    return TYPE_NAME;
}
