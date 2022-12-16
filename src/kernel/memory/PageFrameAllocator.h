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

#ifndef __PAGEFRAMEALLOCATOR_H__
#define __PAGEFRAMEALLOCATOR_H__

#include <cstdint>

#include "TableMemoryManager.h"

namespace Kernel {
class PagingAreaManager;

/**
 * Memory manager, that ist based on the BitmapMemoryManager and is used to manage the page frames in physical memory.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date 2018
 */
class PageFrameAllocator : public TableMemoryManager {

public:
    /**
     * Constructor.
     */
    PageFrameAllocator(PagingAreaManager &pagingAreaManager, uint32_t startAddress, uint32_t endAddress);

    /**
     * Copy Constructor.
     */
    PageFrameAllocator(const PageFrameAllocator &copy) = delete;

    /**
     * Assignment operator.
     */
    PageFrameAllocator& operator=(const PageFrameAllocator &other) = delete;

    /**
     * Destructor.
     */
     ~PageFrameAllocator() override = default;
};

}

#endif
