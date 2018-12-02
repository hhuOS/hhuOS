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
#include <devices/graphics/text/TextDriver.h>
#include <devices/input/Keyboard.h>
#include <kernel/Kernel.h>
#include <kernel/services/InputService.h>
#include <apps/Application.h>
#include <kernel/threads/Scheduler.h>
#include "IOMemoryTestApp.h"

#define NUM_OF_ALLOCS 128

IOMemoryTestApp::IOMemoryTestApp() : Thread("IOMemoryTestApp") {
    random = new Random(20);
    objects = new void*[NUM_OF_ALLOCS];
    stats = new unsigned int[3];
    stats[0] = 0;
    stats[1] = 0;
    stats[2] = 0;
}

IOMemoryTestApp::~IOMemoryTestApp() {
    delete objects;
    delete random;
    delete stats;
}

void IOMemoryTestApp::runTest() {
    unsigned int size = 0;

    for(uint8_t i = 0; i < NUM_OF_ALLOCS; i++) {
        switch(random->rand(3)) {
            case 0: // single page
                size = 1;
                stats[0]++;
                break;
            case 1: // multiple pages (at least 2 and at most 10 pages)
                size = 2 + random->rand(8);
                stats[1]++;
                break;
            case 2: // At least 10 pages
                size = 10 + random->rand();
                stats[2]++;
                break;
            default:break;
        }
        objects[i] = SystemManagement::getInstance().mapIO(size);
    }
    shuffle();

    for (uint8_t i = 0; i < NUM_OF_ALLOCS; i++) {
        SystemManagement::getInstance().freeIO(objects[i]);
    }

}

void IOMemoryTestApp::shuffle() {
    // Shuffle allocated objects
    unsigned int src, dst;
    for (int i = 0; i < 10000; i++) {
        src = random->rand(NUM_OF_ALLOCS);
        dst = random->rand(NUM_OF_ALLOCS);

        void *tmp = objects[dst];
        objects[dst] = objects[src];
        objects[src] = tmp;
    }
}

void IOMemoryTestApp::run() {
    TextDriver *stream = (Kernel::getService<GraphicsService>())->getTextDriver();
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();

    *stream << "===MemoryManagerTest===" << endl;
    *stream << "===Testing IOMemoryManager===" << endl << endl;
    *stream << "Performing " << NUM_OF_ALLOCS << " allocations." << endl << endl;

    uint32_t freeBefore = SystemManagement::getInstance().getIOMemoryManager()->getFreeMemory();
    runTest();
    uint32_t freeAfter = SystemManagement::getInstance().getIOMemoryManager()->getFreeMemory();

    *stream << "Allocs with only a single page page: " << stats[0] << endl;
    *stream << "Allocs with at least 2 and at most 10 pages: " << stats[1] << endl;
    *stream << "Allocs with at least 20 pages: " << stats[2] << endl << endl;

    if(freeBefore - freeAfter != 0) {
        *stream << "*** MEMORY LEAK DETECTED ***" << endl << "Leak: " << (freeBefore - freeAfter) << " Bytes" << endl << endl;
    }

    *stream << "Press [ENTER] to return" << endl;
    while (!kb->isKeyPressed(KeyEvent::RETURN));

    // cleanup
    Application::getInstance().resume();
    Scheduler::getInstance().exit();
}