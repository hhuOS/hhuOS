/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef HHUOS_MEMORYMANAGERTESTAPP_H
#define HHUOS_MEMORYMANAGERTESTAPP_H

#include "kernel/threads/Thread.h"
#include "lib/Random.h"
#include "kernel/memory/manager/BuddyMemoryManager.h"
#include "lib/libc/printf.h"
#include "apps/Application.h"
#include "kernel/Kernel.h"

template <typename T>
class MemoryManagerTest {

private:

    Random random;

    void **objects = nullptr;

    T *manager = nullptr;

    void *memory = nullptr;

    uint32_t numAllocs;

    uint32_t maxAllocSize;

private:

    bool testAlloc();

    void shuffle();

public:

    MemoryManagerTest(uint32_t memorySize, uint32_t numAllocs, uint32_t maxAllocSize = 8192);

    MemoryManagerTest(const MemoryManagerTest &copy) = delete;

    ~MemoryManagerTest();

    void run();
};

template <typename T>
MemoryManagerTest<T>::MemoryManagerTest(uint32_t memorySize, uint32_t numAllocs, uint32_t maxAllocSize) :
        numAllocs(numAllocs), maxAllocSize(maxAllocSize) {
    memory = new char[memorySize];
    manager = new T((uint32_t) memory, ((uint32_t) memory) + memorySize, false);
    objects = new void*[numAllocs];

    random = Random(maxAllocSize - 1);
}

template <typename T>
MemoryManagerTest<T>::~MemoryManagerTest() {
    delete manager;
    delete (char*) memory;
}

template <typename T>
void MemoryManagerTest<T>::run() {
    printf("Start Address: 0x%08x, End Address: 0x%08x\n\n", manager->getStartAddress(), manager->getEndAddress());

    printf("Starting Test:\n\n");
    printf("Allocating %u chunks of memory with a maximum size of %u bytes...", numAllocs, maxAllocSize);

    uint32_t freeBefore = manager->getFreeMemory();

    bool ret = testAlloc();

    if(!ret) {
        return;
    }

    printf(" Finished!\n");
    printf("Freeing allocated memory...");

    uint32_t freeAfter = manager->getFreeMemory();

    printf(" Finished!\n\n");

    if(freeBefore - freeAfter != 0) {
        printf("*** MEMORY LEAK DETECTED ***\nLeak: %u Bytes\n\n", (freeBefore - freeAfter));
    } else {
        printf("Tested alloc successfully!\n");
    }
}

/**
 * Primitive allocations (new calls) and frees
 */
template <typename T>
bool MemoryManagerTest<T>::testAlloc() {
    for(uint8_t i=0; i < numAllocs; i++) {
        objects[i] = manager->alloc(random.rand() + 1);

        if(objects[i] == nullptr) {
            printf("*** OUT OF MEMORY ***\n");
            printf("Test aborted!\n");

            return false;
        }
    }

    shuffle();

    for(uint8_t i=0; i < numAllocs; i++) {
        manager->free(objects[i]);
    }

    return true;
}

/**
 * Shuffles the allocated objects.
 */
template <typename T>
void MemoryManagerTest<T>::shuffle() {
    uint32_t src, dst;

    for (int i = 0; i < 10000; i++) {
        src = random.rand(numAllocs);
        dst = random.rand(numAllocs);

        void *tmp = objects[dst];
        objects[dst] = objects[src];
        objects[src] = tmp;
    }
}

#endif
