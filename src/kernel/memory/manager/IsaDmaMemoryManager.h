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

#ifndef HHUOS_ISADMAMEMORYMANAGER_H
#define HHUOS_ISADMAMEMORYMANAGER_H

#include "BitmapMemoryManager.h"

/**
 * Manages the memory from 0x10000 to 0x90000. hhuOS uses this area for DMA transfers via the ISA bus.
 *
 * This manager allocates blocks with a size of 64KB, as this is the maximum size,
 * that is supported by the ISA controller.
 */
class IsaDmaMemoryManager : public BitmapMemoryManager {

private:

    static const constexpr char *TYPE_NAME = "IsaDmaMemoryManager";

public:

    PROTOTYPE_IMPLEMENT_CLONE(IsaDmaMemoryManager);

	/**
	 * Constructor.
	 */
    IsaDmaMemoryManager();

    /**
     * Overriding function from MemoryManager.
     */
    void init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) override;

    /**
     * Overriding function from MemoryManager.
     */
    String getTypeName() override;

    static const constexpr uint32_t ISA_DMA_BUF_SIZE = 0x10000;
};

#endif
