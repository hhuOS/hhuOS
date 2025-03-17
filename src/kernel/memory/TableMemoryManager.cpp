/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "TableMemoryManager.h"
#include "kernel/log/Log.h"
#include "kernel/memory/BitmapMemoryManager.h"

namespace Kernel {

TableMemoryManager::TableMemoryManager(BitmapMemoryManager &bitmapMemoryManager, uint8_t *startAddress, uint8_t *endAddress, uint32_t blockSize) :
        bitmapMemoryManager(bitmapMemoryManager), startAddress(startAddress), endAddress(endAddress), blockSize(blockSize) {
    uint32_t memorySize = endAddress - startAddress + 1;
    uint32_t blockCount = memorySize / blockSize;
    if (memorySize % blockCount != 0) {
        blockCount++;
    }

    if (bitmapMemoryManager.getBlockSize() < MIN_BITMAP_BLOCK_SIZE) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Bitmap block size is too small!");
    }

    allocationTableCount = (blockCount * sizeof(AllocationTableEntry)) / bitmapMemoryManager.getBlockSize();
    if ((blockCount * sizeof(AllocationTableEntry)) % bitmapMemoryManager.getBlockSize() != 0) {
        allocationTableCount++;
    }

    referenceTableSizeInBlocks = allocationTableCount / (bitmapMemoryManager.getBlockSize() / sizeof(ReferenceTableEntry));
    if (allocationTableCount % (bitmapMemoryManager.getBlockSize() / sizeof(ReferenceTableEntry)) != 0) {
        referenceTableSizeInBlocks++;
    }

    allocationTableEntriesPerBlock = bitmapMemoryManager.getBlockSize() / sizeof(AllocationTableEntry);
    referenceTableEntriesPerBlock = bitmapMemoryManager.getBlockSize() / sizeof(ReferenceTableEntry);
    managedMemoryPerAllocationTable = allocationTableEntriesPerBlock * blockSize;

    referenceTableArray = new ReferenceTableEntry*[referenceTableSizeInBlocks];
    for (uint32_t i = 0; i < referenceTableSizeInBlocks; i++) {
        referenceTableArray[i] = static_cast<ReferenceTableEntry*>(bitmapMemoryManager.allocateBlock());
        for (uint32_t j = 0; j < bitmapMemoryManager.getBlockSize() / sizeof(ReferenceTableEntry); j++) {
            bool installed = (i * (bitmapMemoryManager.getBlockSize() / sizeof(ReferenceTableEntry)) + j) < allocationTableCount;
            auto &entry = referenceTableArray[i][j];
            entry.setAddress(0);
            entry.setInstalled(installed);
        }
    }
}

void TableMemoryManager::setMemory(uint8_t *start, uint8_t *end, uint16_t useCount, bool reserved) {
    auto startIndex = calculateIndex(start);
    auto endIndex = calculateIndex(end);

    for (uint32_t i = startIndex.referenceTableArrayIndex; i <= endIndex.referenceTableArrayIndex; i++) {
        auto *referenceTable = reinterpret_cast<ReferenceTableEntry*>(referenceTableArray[i]);
        uint32_t referenceTableStartIndex = (i == startIndex.referenceTableArrayIndex) ? startIndex.referenceTableIndex : 0;
        uint32_t referenceTableEndIndex = (i == endIndex.referenceTableArrayIndex) ? endIndex.referenceTableIndex : referenceTableEntriesPerBlock - 1;

        for (uint32_t j = referenceTableStartIndex; j <= referenceTableEndIndex; j++) {
            auto &referenceTableEntry = referenceTable[j];
            referenceTableEntry.acquireLock();

            uint32_t address = referenceTableEntry.getAddress();
            if (address == 0) {
                void *block = bitmapMemoryManager.allocateBlock();
                referenceTableEntry.setAddress(reinterpret_cast<uint32_t>(block));
            }

            auto *allocationTable = reinterpret_cast<AllocationTableEntry*>(referenceTableEntry.getAddress());
            uint32_t allocationTableStartIndex = (j == referenceTableStartIndex) ? startIndex.allocationTableIndex : 0;
            uint32_t allocationTableEndIndex = (j == referenceTableEndIndex) ? endIndex.allocationTableIndex : allocationTableEntriesPerBlock - 1;

            for (uint32_t k = allocationTableStartIndex; k <= allocationTableEndIndex; k++) {
                auto &allocationTableEntry = allocationTable[k];
                allocationTableEntry.setReserved(reserved);
                allocationTableEntry.setUseCount(useCount);
            }

            referenceTableEntry.releaseLock();
        }
    }
}

TableMemoryManager::TableIndex TableMemoryManager::calculateIndex(uint8_t *address) const {
    auto *referenceTableAddress = reinterpret_cast<uint8_t*>((reinterpret_cast<uint32_t>(address) / blockSize) * blockSize);
    auto referenceTableIndex = reinterpret_cast<uint32_t>(referenceTableAddress) / managedMemoryPerAllocationTable;

    auto allocationTableAddress = reinterpret_cast<uint8_t*>(reinterpret_cast<uint32_t>(address) % managedMemoryPerAllocationTable);
    auto allocationTableIndex = reinterpret_cast<uint32_t>(allocationTableAddress) / blockSize;

    return { referenceTableIndex / referenceTableEntriesPerBlock, referenceTableIndex % referenceTableEntriesPerBlock, allocationTableIndex };
}

uint32_t TableMemoryManager::calculateAddress(const TableMemoryManager::TableIndex &index) const {
    return index.referenceTableArrayIndex * referenceTableEntriesPerBlock * managedMemoryPerAllocationTable
           + index.referenceTableIndex * managedMemoryPerAllocationTable
           + index.allocationTableIndex * blockSize;
}

void *TableMemoryManager::allocateBlock() {
    return allocateBlockAfterAddress(reinterpret_cast<void*>(startAddress));
}

void *TableMemoryManager::allocateBlockAtAddress(void *address) {
    // TODO: Is this the right way?
    if (address > endAddress) {
        return address;
    }

    const auto index = calculateIndex(static_cast<uint8_t*>(address));

    auto *referenceTable = reinterpret_cast<ReferenceTableEntry*>(referenceTableArray[index.referenceTableArrayIndex]);
    auto &referenceTableEntry = referenceTable[index.referenceTableIndex];

    uint32_t allocationTableAddress = referenceTableEntry.getAddress();
    if (allocationTableAddress == 0) {
        void *block = bitmapMemoryManager.allocateBlock();
        referenceTableEntry.setAddress(reinterpret_cast<uint32_t>(block));
    }

    auto *allocationTable = reinterpret_cast<AllocationTableEntry*>(referenceTableEntry.getAddress());
    auto &allocationTableEntry = allocationTable[index.allocationTableIndex];

    allocationTableEntry.incrementUseCount();
    return reinterpret_cast<void*>(calculateAddress(index));
}

void TableMemoryManager::freeBlock(void *pointer) {
    // TODO: Is this the right way?
    if (pointer > endAddress) {
        return;
    }

    const auto index = calculateIndex(reinterpret_cast<uint8_t*>(pointer));

    auto *referenceTable = reinterpret_cast<ReferenceTableEntry*>(referenceTableArray[index.referenceTableArrayIndex]);
    auto &referenceTableEntry = referenceTable[index.referenceTableIndex];
    auto *allocationTable = reinterpret_cast<AllocationTableEntry*>(referenceTableEntry.getAddress());
    auto &allocationTableEntry = allocationTable[index.allocationTableIndex];

    allocationTableEntry.decrementUseCount();
}

void *TableMemoryManager::allocateBlockAfterAddress(void *address) {
    auto startIndex = calculateIndex(reinterpret_cast<uint8_t*>(address));
    auto endIndex = calculateIndex(endAddress);

    for (uint32_t i = startIndex.referenceTableArrayIndex; i <= endIndex.referenceTableArrayIndex; i++) {
        auto *referenceTable = reinterpret_cast<ReferenceTableEntry*>(referenceTableArray[i]);
        uint32_t referenceTableStartIndex = (i == startIndex.referenceTableArrayIndex) ? startIndex.referenceTableIndex : 0;
        uint32_t referenceTableEndIndex = (i == endIndex.referenceTableArrayIndex) ? endIndex.referenceTableIndex : referenceTableEntriesPerBlock - 1;

        for (uint32_t j = referenceTableStartIndex; j <= referenceTableEndIndex; j++) {
            auto &referenceTableEntry = referenceTable[j];
            if (!referenceTableEntry.tryAcquireLock()) {
                continue;
            }

            uint32_t allocationTableAddress = referenceTableEntry.getAddress();
            if (allocationTableAddress == 0) {
                void *block = bitmapMemoryManager.allocateBlock();
                referenceTableEntry.setAddress(reinterpret_cast<uint32_t>(block));
            }

            auto *allocationTable = reinterpret_cast<AllocationTableEntry*>(referenceTableEntry.getAddress());
            uint32_t allocationTableStartIndex = (j == referenceTableStartIndex) ? startIndex.allocationTableIndex : 0;
            uint32_t allocationTableEndIndex = (j == referenceTableEndIndex) ? endIndex.allocationTableIndex : allocationTableEntriesPerBlock - 1;

            for (uint32_t k = allocationTableStartIndex; k <= allocationTableEndIndex; k++) {
                auto &allocationTableEntry = allocationTable[k];
                if (allocationTableEntry.isReserved() || allocationTableEntry.getUseCount() > 0) {
                    continue;
                }

                allocationTableEntry.incrementUseCount();
                referenceTableEntry.releaseLock();
                const TableIndex index = {i, j, k};
                return reinterpret_cast<void*>(calculateAddress(index));
            }

            referenceTableEntry.releaseLock();
        }
    }

    return nullptr;
}

uint32_t TableMemoryManager::getTotalMemory() const {
    return endAddress - startAddress + 1;
}

uint32_t TableMemoryManager::getBlockSize() const {
    return blockSize;
}

uint32_t TableMemoryManager::getFreeMemory() const {
    uint32_t freeMemory = 0;

    for (uint32_t i = 0; i < referenceTableSizeInBlocks; i++) {
        for (uint32_t j = 0; j < bitmapMemoryManager.getBlockSize() / sizeof(ReferenceTableEntry); j++) {
            auto referenceTableEntry = referenceTableArray[i][j];
            if (!referenceTableEntry.isInstalled()) {
                continue;
            }

            if (referenceTableEntry.getAddress() == 0) {
                freeMemory += managedMemoryPerAllocationTable;
                continue;
            }

            auto *allocationTable = reinterpret_cast<AllocationTableEntry*>(referenceTableEntry.getAddress());
            for (uint32_t k = 0; k < allocationTableEntriesPerBlock; k++) {
                auto allocationTableEntry = allocationTable[k];
                if (allocationTableEntry.getUseCount() == 0) {
                    freeMemory += blockSize;
                }
            }
        }
    }

    return freeMemory;
}

uint8_t * TableMemoryManager::getStartAddress() const {
    return startAddress;
}

uint8_t * TableMemoryManager::getEndAddress() const {
    return endAddress;
}

void TableMemoryManager::debugLog() {
    uint32_t memorySize = endAddress - startAddress + 1;
    uint32_t blockCount = memorySize / blockSize;

    LOG_DEBUG("startAddress: [%u]", startAddress);
    LOG_DEBUG("endAddress: [%u]", endAddress);
    LOG_DEBUG("blockSize: [%u]", blockSize);
    LOG_DEBUG("memorySize: [%u]", memorySize);
    LOG_DEBUG("blockCount: [%u]", blockCount);
    LOG_DEBUG("bitmapBlockSize: [%u]", bitmapMemoryManager.getBlockSize());
    LOG_DEBUG("allocationTableCount: [%u]", allocationTableCount);
    LOG_DEBUG("allocationTableEntriesPerBlock: [%u]", allocationTableEntriesPerBlock);
    LOG_DEBUG("managedMemoryPerAllocationTable: [%u]", managedMemoryPerAllocationTable);
    LOG_DEBUG("referenceTableEntriesPerBlock: [%u]", referenceTableEntriesPerBlock);
    LOG_DEBUG("referenceTableSizeInBlocks: [%u]", referenceTableSizeInBlocks);

    for (uint32_t i = 0; i < referenceTableSizeInBlocks; i++) {
        for (uint32_t j = 0; j < bitmapMemoryManager.getBlockSize() / sizeof(ReferenceTableEntry); j++) {
            auto entry = referenceTableArray[i][j];
            LOG_DEBUG("Index: [%04u], Allocation Table Address: [0x%08x], Installed: [%B], Locked: [%B]", i * j + j, entry.getAddress(), entry.isInstalled(), entry.isLocked());
            if (entry.getAddress() > 0) {
                printAllocationTable(i, j);
            }
        }
    }
}

void TableMemoryManager::printAllocationTable(uint32_t referenceTableArrayIndex, uint32_t referenceTableIndex) {
    auto referenceTableEntry = referenceTableArray[referenceTableArrayIndex][referenceTableIndex];
    auto *allocationTable = reinterpret_cast<AllocationTableEntry*>(referenceTableEntry.getAddress());
    for (uint32_t i = 0; i < allocationTableEntriesPerBlock; i++) {
        auto entry = allocationTable[i];
        uint32_t address = referenceTableArrayIndex * referenceTableEntriesPerBlock * managedMemoryPerAllocationTable + referenceTableIndex * managedMemoryPerAllocationTable + i * blockSize;
        LOG_DEBUG("Index: [%04u], Address: [0x%08x], Used: [%04u], Reserved: [%B]", i, address, entry.getUseCount(), entry.isReserved());
    }
}

}