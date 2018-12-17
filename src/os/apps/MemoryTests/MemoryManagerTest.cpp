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

#include <devices/input/Keyboard.h>
#include <kernel/services/InputService.h>
#include "MemoryManagerTest.h"

MemoryManagerTest::MemoryManagerTest(const String &managerType, uint32_t memorySize, uint32_t numAllocs,
                                     uint32_t maxAllocSize, const String &name) :
        manager(*((MemoryManager *) MemoryManager::createInstance(managerType))), managerName(name),
        deleteManager(true), numAllocs(numAllocs), maxAllocSize(maxAllocSize) {
    memory = new char[memorySize];
    manager.init((uint32_t) memory, (uint32_t) memory + memorySize, false);
    objects = new void*[numAllocs];

    random = Random(maxAllocSize - 1);
}

MemoryManagerTest::MemoryManagerTest(MemoryManager &manager, uint32_t memorySize, uint32_t numAllocs,
                                     uint32_t maxAllocSize, const String &name) :
        manager(manager), managerName(name), numAllocs(numAllocs), maxAllocSize(maxAllocSize) {
    objects = new void*[numAllocs];

    random = Random(maxAllocSize - 1);
}

MemoryManagerTest::~MemoryManagerTest() {
    if(deleteManager) {
        delete &manager;
        delete (char*) memory;
    }

    delete objects;
}

void MemoryManagerTest::run() {
    printf("===MemoryManagerTest===\n");
    printf("===Testing %s===\n\n", (const char*) (managerName.isEmpty() ? manager.getTypeName() : managerName));

    printf("Start Address: 0x%08x, End Address: 0x%08x\n\n", manager.getStartAddress(), manager.getEndAddress());

    printf("Starting Test:\n\n");
    printf("Allocating %u chunks of memory with a maximum size of %u bytes...", numAllocs, maxAllocSize);

    uint32_t freeBefore = manager.getFreeMemory();

    bool ret = testAlloc();

    if(ret) {
        printf(" Finished!\n");
        printf("Freeing allocated memory...");

        uint32_t freeAfter = manager.getFreeMemory();

        printf(" Finished!\n\n");

        if (freeBefore - freeAfter != 0) {
            printf("*** MEMORY LEAK DETECTED ***\nLeak: %u Bytes\n", (freeBefore - freeAfter));
        } else {
            printf("Tested alloc successfully!\n");
        }
    }

    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();

    printf("\nPress [ENTER] to return");
    while (!kb->isKeyPressed(KeyEvent::RETURN));
    while (kb->isKeyPressed(KeyEvent::RETURN));
}

bool MemoryManagerTest::testAlloc() {
    for(uint8_t i=0; i < numAllocs; i++) {
        objects[i] = manager.alloc(random.rand() + 1);

        if(objects[i] == nullptr) {
            printf("\n\n*** OUT OF MEMORY ***\n");
            printf("Test aborted!\n");

            return false;
        }
    }

    shuffle();

    for(uint8_t i=0; i < numAllocs; i++) {
        manager.free(objects[i]);
    }

    return true;
}

void MemoryManagerTest::shuffle() {
    uint32_t src, dst;

    for (int i = 0; i < 10000; i++) {
        src = random.rand(numAllocs);
        dst = random.rand(numAllocs);

        void *tmp = objects[dst];
        objects[dst] = objects[src];
        objects[src] = tmp;
    }
}