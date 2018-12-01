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

#ifndef __PAGEFRAMEALLOCATOR_H__
#define __PAGEFRAMEALLOCATOR_H__

#include <cstdint>
#include "kernel/memory/manager/BitmapMemoryManager.h"

/**
 * Memory manager, that ist based on the BitmapMemoryManager and is used to manage the page frames in physical memory.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2018
 */
class PageFrameAllocator : public BitmapMemoryManager{

public:

	/**
	 * Constructor.
	 *
	 * @param memoryStartAddress Start address of the memory area to manage
	 * @param memoryEndAddress End address of the memory area to manage
	 */
    PageFrameAllocator(uint32_t memoryStartAddress, uint32_t memoryEndAddress);

};

#endif
