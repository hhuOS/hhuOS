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

#include <kernel/memory/SystemManagement.h>
#include <kernel/threads/Scheduler.h>
#include <apps/Application.h>
#include <kernel/Kernel.h>
#include <kernel/log/Logger.h>
#include "HeapTestApp.h"

#define NUM_OF_ALLOCS 128

HeapTestApp::HeapTestApp() : Thread("HeapTestApp") {
    random = new Random(4096 * 16);
    objects = new unsigned int*[NUM_OF_ALLOCS];
    stats = new unsigned int[3];
    stats[0] = 0;
    stats[1] = 0;
    stats[2] = 0;
}

HeapTestApp::~HeapTestApp() {
    delete random;
    delete objects;
    delete stats;
}

void HeapTestApp::performAllocs() {
    unsigned int size = 0;
    for(uint8_t i=0; i < NUM_OF_ALLOCS; i++) {
        switch(random->rand(3)) {
            case 0:
                size = random->rand(50);
                stats[0]++;
                break;
            case 1:
                size = 50 + random->rand(50);
                stats[1]++;
                break;
            case 2:
                size = 100 + random->rand();
                stats[2]++;
                break;
            default:break;
        }
        objects[i] = (unsigned int*) new char[size];
    }

    shuffle();

    for(uint8_t i=0; i < NUM_OF_ALLOCS; i++) {
        delete[] objects[i];
    }
}

void HeapTestApp::shuffle() {
    unsigned int src, dst;
    for (int i = 0; i < 10000; i++) {
        src = random->rand(NUM_OF_ALLOCS);
        dst = random->rand(NUM_OF_ALLOCS);

        unsigned int *tmp = objects[dst];
        objects[dst] = objects[src];
        objects[src] = tmp;
    }
}

void HeapTestApp::run() {
    TextDriver *stream = (Kernel::getService<GraphicsService>())->getTextDriver();
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();

    *stream << "===MemoryManagerTest===" << endl;
    *stream << "===Testing kernel heap===" << endl << endl;
    *stream << "Performing " << NUM_OF_ALLOCS << " allocations." << endl << endl;

    uint32_t freeBefore = SystemManagement::getKernelHeapManager()->getFreeMemory();
    performAllocs();
    uint32_t freeAfter = SystemManagement::getKernelHeapManager()->getFreeMemory();

    *stream << "Small Allocs: " << stats[0] << " (Less then 1024 Bytes)" << endl;
    *stream << "Medium Allocs: " << stats[1] << " (Between 1024 and 4096 Bytes)" << endl;
    *stream << "Large Allocs: " << stats[2] << " (At least 4096 Bytes)" << endl << endl;

    if(freeBefore - freeAfter != 0) {
        *stream << "*** MEMORY LEAK DETECTED ***" << endl << "Leak: " << (freeBefore - freeAfter) << " Bytes" << endl << endl;
    }

    *stream << "Press [ENTER] to return" << endl;
    while (!kb->isKeyPressed(KeyEvent::RETURN));

    // cleanup
    Application::getInstance().resume();
    Scheduler::getInstance().exit();
}
