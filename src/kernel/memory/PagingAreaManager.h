/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include <stdint.h>

#include "lib/util/collection/Pool.h"
#include "lib/util/base/BitmapMemoryManager.h"

namespace Kernel {

/**
 * Memory manager, that ist based on the BitmapMemoryManager and is used to manage the part of virtual memory,
 * that is reserved for page tables and directories.
 *
 * @author Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * @date HHU, 2018
 */
class PagingAreaManager : public Util::BitmapMemoryManager {

public:
    /**
     * Constructor.
     */
    PagingAreaManager(uint8_t *startAddress, uint32_t mappedPages, uint32_t bootstrapPageCount);

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

     void refillPool();

private:

    Util::Pool<void> blockPool;

    static const constexpr uint32_t BLOCK_POOL_SIZE = 128;

};

}

#endif
