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

#ifndef __BITMAPMEMORYMANAGER_H__
#define __BITMAPMEMORYMANAGER_H__

#include <cstdint>

#include "lib/util/memory/AtomicBitmap.h"
#include "BlockMemoryManager.h"

namespace Kernel {

/**
 * Memory manager, that manages a given area of memory in blocks of given size using a bitmap mechanism.
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */
class BitmapMemoryManager : public BlockMemoryManager {

public:
    /**
     * Constructor.
     */
    BitmapMemoryManager(uint32_t startAddress, uint32_t endAddress, uint32_t blockSize = 4096, bool zeroMemory = false);

    /**
     * Copy Constructor.
     */
    BitmapMemoryManager(const BitmapMemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    BitmapMemoryManager& operator=(const BitmapMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~BitmapMemoryManager() override = default;

    [[nodiscard]] void *allocateBlock() override;

    void freeBlock(void *pointer) override;

    virtual void handleError();

    [[nodiscard]] uint32_t getTotalMemory() const override;

    [[nodiscard]] uint32_t getFreeMemory() const override;

    [[nodiscard]] uint32_t getBlockSize() const override;

    [[nodiscard]] uint32_t getStartAddress() const override;

    [[nodiscard]] uint32_t getEndAddress() const override;

protected:

    void setRange(uint32_t startBlock, uint32_t blockCount);

private:

    uint32_t startAddress;
    uint32_t endAddress;
    uint32_t freeMemory;

    uint32_t blockSize;
    bool zeroMemory = false;

    Util::Memory::AtomicBitmap bitmap;

};

}

#endif
