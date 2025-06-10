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

#ifndef HHUOS_SLABALLOCATOR_H
#define HHUOS_SLABALLOCATOR_H

#include <stdint.h>

#include "lib/util/base/BitmapMemoryManager.h"
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

    [[nodiscard]] void* getStartAddress() const override;

    [[nodiscard]] void* getEndAddress() const override;

    static const constexpr uint32_t MAX_SLAB_SIZE = 128 * 1024;

private:

    Util::BitmapMemoryManager pool4K;
    Util::BitmapMemoryManager pool8K;
    Util::BitmapMemoryManager pool16K;
    Util::BitmapMemoryManager pool32K;
    Util::BitmapMemoryManager pool64K;
    Util::BitmapMemoryManager pool128K;

    static const constexpr uint32_t POOL_SIZE = 1 * 1024 * 1024;
};

}

#endif
