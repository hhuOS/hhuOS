/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_TABLEMEMORYMANAGER_H
#define HHUOS_TABLEMEMORYMANAGER_H

#include <stdint.h>

#include "lib/util/async/Atomic.h"
#include "lib/util/base/BlockMemoryManager.h"
#include "lib/util/base/Panic.h"

namespace Util {
class BitmapMemoryManager;
}

namespace Kernel {

class TableMemoryManager : public Util::BlockMemoryManager {

public:
    /**
     * Constructor.
     */
    TableMemoryManager(Util::BitmapMemoryManager &bitmapMemoryManager, uint8_t *startAddress, uint8_t *endAddress, uint32_t blockSize = 4096);

    /**
     * Copy Constructor.
     */
    TableMemoryManager(const TableMemoryManager &copy) = delete;

    /**
     * Assignment operator.
     */
    TableMemoryManager& operator=(const TableMemoryManager &other) = delete;

    /**
     * Destructor.
     */
    ~TableMemoryManager() override = default;

    void setMemory(uint8_t *start, uint8_t *end, uint16_t useCount, bool reserved);

    void* allocateBlock() override;

    void* allocateBlockAtAddress(void *address);

    void* allocateBlockAfterAddress(void *address);

    void freeBlock(void *pointer) override;

    uint32_t getTotalMemory() const override;

    uint32_t getBlockSize() const override;

    uint32_t getFreeMemory() const override;

    void* getStartAddress() const override;

    void* getEndAddress() const override;

    void debugLog();

private:

    void printAllocationTable(uint32_t referenceTableArrayIndex, uint32_t referenceTableIndex);

    struct ReferenceTableEntry {

    private:
        /**
         * 0-1:    unused
         * 2:      lock
         * 3:      installed
         * 4-31:   address
         */
        uint32_t value;

    public:

        void setAddress(uint32_t address) {
            auto valueWrapper = Util::Async::Atomic<uint32_t>(value);
            uint32_t oldValue;
            uint32_t exchangeValue;

            do {
                oldValue = valueWrapper.get();
                exchangeValue = (oldValue & 0x0000000f) | (address & 0xfffffff0);
            } while (valueWrapper.getAndSet(exchangeValue) != oldValue);
        }

        void setInstalled(bool installed) {
            auto valueWrapper = Util::Async::Atomic<uint32_t>(value);
            if (installed) {
                valueWrapper.bitSet(3);
            } else {
                valueWrapper.bitUnset(3);
            }
        }

        void acquireLock() {
            while (!tryAcquireLock());
        }

        bool tryAcquireLock() {
            auto valueWrapper = Util::Async::Atomic<uint32_t>(value);
            return !valueWrapper.bitTestAndSet(2);
        }

        void releaseLock() {
            auto valueWrapper = Util::Async::Atomic<uint32_t>(value);
            valueWrapper.bitUnset(2);
        }

        uint32_t getAddress() {
            auto valueWrapper = Util::Async::Atomic<uint32_t>(value);
            return valueWrapper.get() & 0xfffffff0;
        }

        bool isInstalled() {
            auto valueWrapper = Util::Async::Atomic<uint32_t>(value);
            return valueWrapper.bitTest(3);
        }

        bool isLocked() {
            auto valueWrapper = Util::Async::Atomic<uint32_t>(value);
            return valueWrapper.bitTest(2);
        }
    };

    struct AllocationTableEntry {

    private:
        /**
         * 0:       reserved memory
         * 1-15:    useCount
         */
        uint16_t value;

    public:

        void setReserved(bool reserved) {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            if (reserved) {
                valueWrapper.bitSet(0);
            } else {
                valueWrapper.bitUnset(0);
            }
        }

        void setUseCount(uint16_t useCount) {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            uint16_t oldValue;
            uint16_t exchangeValue;

            do {
                oldValue = valueWrapper.get();
                exchangeValue = (oldValue & 0x0001) | (useCount << 1);
            } while (valueWrapper.getAndSet(exchangeValue) != oldValue);
        }

        void incrementUseCount() {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            uint16_t oldValue = valueWrapper.fetchAndAdd(0b0000000000000010);
            if (static_cast<uint16_t>(oldValue + 2) <= oldValue) {
                Util::Panic::fire(Util::Panic::Error::PAGING_ERROR, "Page frame has been mapped too often!");
            }
        }

        void decrementUseCount() {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            uint16_t oldValue = valueWrapper.fetchAndSub(0b0000000000000010);
            if (static_cast<uint16_t>(oldValue - 2) >= oldValue) {
                Util::Panic::fire(Util::Panic::Error::PAGING_ERROR, "Page frame underflow!");
            }
        }

        uint16_t getUseCount() {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            return valueWrapper.get() >> 1;
        }

        bool isReserved() {
            auto valueWrapper = Util::Async::Atomic<uint16_t>(value);
            return valueWrapper.bitTest(0);
        }
    };

    struct TableIndex {
        uint32_t referenceTableArrayIndex;
        uint32_t referenceTableIndex;
        uint32_t allocationTableIndex;
    };

    TableIndex calculateIndex(uint8_t *address) const;

    uint32_t calculateAddress(const TableIndex &index) const;

private:

    Util::BitmapMemoryManager &bitmapMemoryManager;
    uint8_t *startAddress;
    uint8_t *endAddress;
    uint32_t blockSize;

    uint32_t referenceTableSizeInBlocks;
    uint32_t allocationTableEntriesPerBlock;
    uint32_t managedMemoryPerAllocationTable;
    uint32_t referenceTableEntriesPerBlock;
    uint32_t allocationTableCount;

    ReferenceTableEntry **referenceTableArray;
    static const constexpr uint32_t MIN_BITMAP_BLOCK_SIZE = 16;
};

}

#endif
