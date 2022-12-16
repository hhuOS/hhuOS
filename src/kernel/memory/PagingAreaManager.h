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

#ifndef __PAGINGAREAMANAGER_H__
#define __PAGINGAREAMANAGER_H__

#include <cstdint>

#include "lib/util/data/Pool.h"
#include "kernel/memory/BitmapMemoryManager.h"

namespace Kernel {

/**
 * Memory manager, that ist based on the BitmapMemoryManager and is used to manage the part of virtual memory,
 * that is reserved for page tables and directories.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class PagingAreaManager : public BitmapMemoryManager {

public:
    /**
     * Constructor.
     */
    PagingAreaManager();

    /**
     * Copy Constructor.
     */
    PagingAreaManager(const PagingAreaManager &copy) = delete;

    /**
     * Assignment operator.
     */
    PagingAreaManager& operator=(const PagingAreaManager &other) = delete;

    /**
     * Destructor.
     */
     ~PagingAreaManager() override = default;

    [[nodiscard]] void* allocateBlock() override;

     void freeBlock(void *pointer) override;

     void handleError() override;

     void refillPool();

private:

    Util::Data::Pool<void> blockPool;

    static const constexpr uint32_t BLOCK_POOL_SIZE = 128;

};

}

#endif
