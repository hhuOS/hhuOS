/**
 * IOMemoryTestApp - tests the io memory area with a certain number of allocations and frees
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */

#include <kernel/memory/SystemManagement.h>
#include <devices/graphics/text/TextDriver.h>
#include <devices/input/Keyboard.h>
#include <kernel/Kernel.h>
#include <kernel/services/InputService.h>
#include <user/Application.h>
#include <kernel/threads/Scheduler.h>
#include "IOMemoryTestApp.h"

#define NUM_OF_ALLOCS 128

/**
 * Constructor
 */
IOMemoryTestApp::IOMemoryTestApp() : Thread("IOMemoryTestApp") {
    random = new Random(20);
    objects = new IOMemInfo[NUM_OF_ALLOCS];
    stats = new unsigned int[3];
    stats[0] = 0;
    stats[1] = 0;
    stats[2] = 0;
}

/**
 * Destructor
 */
IOMemoryTestApp::~IOMemoryTestApp() {
    delete objects;
    delete random;
    delete stats;
}

/**
 * Allocates and frees memory in the IO-Area.
 */
void IOMemoryTestApp::runTest() {
    //SystemManagement::getInstance()->dumpFreeIOMemBlocks();

    unsigned int size = 0;
    for(uint8_t i=0; i < NUM_OF_ALLOCS; i++) {
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
        objects[i] = SystemManagement::getInstance()->mapIO(size);
    }
    shuffle();
    //SystemManagement::getInstance()->dumpFreeIOMemBlocks();

    for (uint8_t i = 0; i < NUM_OF_ALLOCS; i++) {
        SystemManagement::getInstance()->freeIO(objects[i]);
    }
    //SystemManagement::getInstance()->dumpFreeIOMemBlocks();

}

/**
 * Shuffles the allocated objects.
 */
void IOMemoryTestApp::shuffle() {
    // Shuffle allocated objects
    unsigned int src, dst;
    for (int i = 0; i < 10000; i++) {
        src = random->rand(NUM_OF_ALLOCS);
        dst = random->rand(NUM_OF_ALLOCS);

        IOMemInfo tmp = objects[dst];
        objects[dst] = objects[src];
        objects[src] = tmp;
    }
}

/**
 * Thread run method.
 */
void IOMemoryTestApp::run() {
    Cpu::disableInterrupts();

    TextDriver *stream = (Kernel::getService<GraphicsService>())->getTextDriver();
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();
    *stream << "_____ IOMemory Demo App _____" << endl << endl;

    uint32_t freeBefore = SystemManagement::getInstance()->getFreeIoMemoryAmount();
    runTest();
    uint32_t freeAfter = SystemManagement::getInstance()->getFreeIoMemoryAmount();
    *stream << "Small page allocs: " << stats[0] << endl;
    *stream << "At least 2 and at most 10 pages: " << stats[1] << endl;
    *stream << "At least 20 pages: " << stats[2] << endl << endl;
    if(freeAfter - freeBefore != 0) {
        *stream << "Ups. Lost a few bytes. Leak: " << (freeAfter - freeBefore) << " Bytes" << endl;
    }

    Cpu::enableInterrupts();
    *stream << "Press [ENTER] to return" << endl;
    while (!kb->isKeyPressed(KeyEvent::RETURN));

    // cleanup
    Application::getInstance()->resume();
    Scheduler::getInstance()->exit();
}