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

#ifndef HHUOS_BLOCKMEMORYMANAGER_H
#define HHUOS_BLOCKMEMORYMANAGER_H

#include "lib/util/base/MemoryManager.h"

namespace Kernel {

class BlockMemoryManager : public Util::MemoryManager {

public:
    /**
     * Constructor.
     */
    BlockMemoryManager() = default;

    /**
     * Copy Constructor.
     */
    BlockMemoryManager(const BlockMemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    BlockMemoryManager &operator=(const BlockMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~BlockMemoryManager() override = default;

    [[nodiscard]] virtual void *allocateBlock() = 0;

    virtual void freeBlock(void *pointer) = 0;

    [[nodiscard]] virtual uint32_t getBlockSize() const = 0;
};

}

#endif
