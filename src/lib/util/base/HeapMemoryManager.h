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

#ifndef HHUOS_LIB_UTIL_HEAPMEMORYMANAGER_H
#define HHUOS_LIB_UTIL_HEAPMEMORYMANAGER_H

#include <stddef.h>

#include "base/MemoryManager.h"
#include "reflection/Prototype.h"

namespace Util {

/// Base class for heap memory managers.
/// A heap memory manager must support allocation, reallocation and freeing of memory.
class HeapMemoryManager : public MemoryManager {

public:
    /// The base heap memory manager class has no state, so the default constructor is sufficient.
    HeapMemoryManager() = default;

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    HeapMemoryManager(const HeapMemoryManager &copy) = delete;

	/// A memory manager should not be copyable, since copies would operate on the same memory.
    HeapMemoryManager &operator=(const HeapMemoryManager &other) = delete;

    /// The base heap memory manager class has no state, so the default destructor is sufficient.
    ~HeapMemoryManager() override = default;

    /// Allocate a block of memory of a given size and alignment.
    /// If no sufficient block of memory is available, nullptr is returned.
    [[nodiscard]] virtual void* allocateMemory(size_t size, size_t alignment) = 0;

	/// Reallocate a previously allocated block of memory to a new size and alignment.
	/// If a new chunk needs to be allocated for the reallocation, the old chunk is freed and
	/// the content of the old chunk is copied into the new one up to the lesser of the new and old sizes.
    [[nodiscard]] virtual void* reallocateMemory(void *pointer, size_t size, size_t alignment) = 0;

    /// Free a block of memory that was previously allocated by this memory manager.
    virtual void freeMemory(void *pointer, size_t alignment) = 0;

    /// If the manager uses a locking mechanism, this method returns the lock state.
    [[nodiscard]] virtual bool isLocked() const = 0;
};

}

#endif
