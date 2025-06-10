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

#ifndef HHUOS_LIB_UTIL_BLOCKMEMORYMANAGER_H
#define HHUOS_LIB_UTIL_BLOCKMEMORYMANAGER_H

#include <stddef.h>

#include "base/MemoryManager.h"

namespace Util {

/// Base class for memory managers that manage memory in fixed-size blocks.
/// A block memory manager must support allocation and freeing of single blocks of memory.
/// They are for example used to manage page frames or stack memory.
class BlockMemoryManager : public MemoryManager {

public:
    /// The base block memory manager class has no state, so the default constructor is sufficient.
    BlockMemoryManager() = default;

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    BlockMemoryManager(const BlockMemoryManager &copy) = delete;

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    BlockMemoryManager &operator=(const BlockMemoryManager &other) = delete;

    /// The base block memory manager class has no state, so the default destructor is sufficient.
    ~BlockMemoryManager() override = default;

    /// Allocate a block of memory of the size defined by the block size of this manager.
    [[nodiscard]] virtual void* allocateBlock() = 0;

    /// Free a block of memory that was previously allocated by this memory manager.
    virtual void freeBlock(void *pointer) = 0;

    /// Return the size of a single block managed by this memory manager.
    [[nodiscard]] virtual size_t getBlockSize() const = 0;
};

}

#endif
