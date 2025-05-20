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

#ifndef HHUOS_LIB_UTIL_MEMORYMANAGER_H
#define HHUOS_LIB_UTIL_MEMORYMANAGER_H

#include <stddef.h>

namespace Util {

/// Base class for memory managers.
class MemoryManager {

public:
    /// The base memory manager class has no state, so the default constructor is sufficient.
    MemoryManager() = default;

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    MemoryManager(const MemoryManager &copy) = delete;

    /// A memory manager should not be copyable, since copies would operate on the same memory.
    MemoryManager& operator=(const MemoryManager &other) = delete;

    /// The base memory manager class has no state, so the default destructor is sufficient.
    virtual ~MemoryManager() = default;

    /// Get the total amount of memory managed by this memory manager.
    [[nodiscard]] virtual size_t getTotalMemory() const = 0;

    /// Get the amount of free memory left in this memory manager.
    [[nodiscard]] virtual size_t getFreeMemory() const = 0;

    /// Get the start address of the managed memory.
    [[nodiscard]] virtual void* getStartAddress() const = 0;

    /// Get the end address of the managed memory.
    [[nodiscard]] virtual void* getEndAddress() const = 0;
};

}

#endif
