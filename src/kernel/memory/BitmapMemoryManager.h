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

#ifndef __BITMAPMEMORYMANAGER_H__
#define __BITMAPMEMORYMANAGER_H__

#include <stdint.h>

#include "lib/util/async/AtomicBitmap.h"
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
    BitmapMemoryManager(uint8_t *startAddress, uint8_t *endAddress, uint32_t blockSize = 4096, bool zeroMemory = false);

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

    [[nodiscard]] uint32_t getTotalMemory() const override;

    [[nodiscard]] uint32_t getFreeMemory() const override;

    [[nodiscard]] uint32_t getBlockSize() const override;

    [[nodiscard]] void* getStartAddress() const override;

    [[nodiscard]] void* getEndAddress() const override;

protected:

    void setRange(uint32_t startBlock, uint32_t blockCount);

private:

    uint8_t *startAddress;
    uint8_t *endAddress;
    uint32_t freeMemory;

    uint32_t blockSize;
    bool zeroMemory = false;

    Util::Async::AtomicBitmap bitmap;

};

}

#endif
