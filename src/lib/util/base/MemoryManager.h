/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_MEMORYMANAGER_H
#define HHUOS_MEMORYMANAGER_H

namespace Util {

class MemoryManager {

public:
    /**
     * Constructor.
     */
    MemoryManager() = default;

    /**
     * Copy Constructor.
     */
    MemoryManager(const MemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    MemoryManager &operator=(const MemoryManager &other) = delete;

    /**
     * Destructor.
     */
    virtual ~MemoryManager() = default;

    /**
     * Get the total amount of memory.
     */
    [[nodiscard]] virtual uint32_t getTotalMemory() const = 0;

    /**
     * Get the amount of free memory.
     */
    [[nodiscard]] virtual uint32_t getFreeMemory() const = 0;

    /**
     * Get the start address of the managed memory.
     */
    [[nodiscard]] virtual uint32_t getStartAddress() const = 0;

    /**
     * Get the end address of the managed memory.
     */
    [[nodiscard]] virtual uint32_t getEndAddress() const = 0;
};

}

#endif
