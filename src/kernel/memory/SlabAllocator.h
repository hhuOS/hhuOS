/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_SLABALLOCATOR_H
#define HHUOS_SLABALLOCATOR_H

#include <stdint.h>

#include "BitmapMemoryManager.h"
#include "lib/util/base/MemoryManager.h"

namespace Kernel {

class SlabAllocator : public Util::MemoryManager {

public:
    /**
     * Default Constructor.
     */
    explicit SlabAllocator(uint8_t *slabMemory);

    /**
     * Copy Constructor.
     */
    SlabAllocator(const SlabAllocator &other) = delete;

    /**
     * Assignment operator.
     */
    SlabAllocator &operator=(const SlabAllocator &other) = delete;

    /**
     * Destructor.
     */
    ~SlabAllocator() override = default;

    void *allocateBlock(uint32_t frameCount);

    bool freeBlock(void *pointer);

    [[nodiscard]] uint32_t getTotalMemory() const override;

    [[nodiscard]] uint32_t getFreeMemory() const override;

    [[nodiscard]] uint8_t* getStartAddress() const override;

    [[nodiscard]] uint8_t* getEndAddress() const override;

    static const constexpr uint32_t MAX_SLAB_SIZE = 128 * 1024;

private:

    BitmapMemoryManager pool4K;
    BitmapMemoryManager pool8K;
    BitmapMemoryManager pool16K;
    BitmapMemoryManager pool32K;
    BitmapMemoryManager pool64K;
    BitmapMemoryManager pool128K;

    static const constexpr uint32_t POOL_SIZE = 1 * 1024 * 1024;
};

}

#endif
