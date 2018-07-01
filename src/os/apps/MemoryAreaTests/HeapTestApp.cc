/**
 * HeapTestApp - tests the kernel heap with a certain number of allocations and frees
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */

#include <kernel/memory/SystemManagement.h>
#include <lib/Random.h>
#include <kernel/threads/Scheduler.h>
#include <apps/Application.h>
#include <kernel/Kernel.h>
#include <kernel/log/Logger.h>
#include "HeapTestApp.h"

#define NUM_OF_ALLOCS 128

Logger &HeapTestApp::log = Logger::get("HEAP_TEST");

/**
 * Constructor
 */
HeapTestApp::HeapTestApp() : Thread("HeapTestApp") {
    random = new Random(4096 * 16);
    objects = new unsigned int*[NUM_OF_ALLOCS];
    stats = new unsigned int[3];
    stats[0] = 0;
    stats[1] = 0;
    stats[2] = 0;
}

/**
 * Destructor
 */
HeapTestApp::~HeapTestApp() {
    delete random;
    delete objects;
    delete stats;
}

/**
 * Primitive allocations (new calls) and frees
 */
void HeapTestApp::primitiveAllocs() {
    unsigned int size = 0;
    for(uint8_t i=0; i < NUM_OF_ALLOCS; i++) {
        switch(random->rand(3)) {
            case 0: // small alloc (less than 1024 Bytes)
                size = random->rand(1024);
                stats[0]++;
                break;
            case 1: // medium alloc (between 1024 and 4096 Bytes)
                size = 1024 + random->rand(3072);
                stats[1]++;
                break;
            case 2: // big alloc (at least a 4096 bytes)
                size = 4096 + random->rand();
                stats[2]++;
                break;
            default:break;
        }
        objects[i] = (unsigned int*) new char(size);
    }
    shuffle();
    for(uint8_t i=0; i < NUM_OF_ALLOCS; i++) {
        delete objects[i];
    }
}


/**
 * Array allocations (new[] calls) and frees
 */
void HeapTestApp::arrayAllocs() {
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
        objects[i] = (unsigned int*) new char(size);
    }
    shuffle();
    for(uint8_t i=0; i < NUM_OF_ALLOCS; i++) {
        delete[] objects[i];
    }
}

/**
 * Shuffles the allocated objects.
 */
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

/**
 * Thread run method.
 */
void HeapTestApp::run() {
     log.trace("Running HeapTestApp");

    TextDriver *stream = (Kernel::getService<GraphicsService>())->getTextDriver();
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();
    *stream << "_____ Heap Demo App _____" << endl;
    *stream << "Tests new and new[] calls - each alloc type is called " << NUM_OF_ALLOCS << " times." << endl << endl;

     log.trace("Testing primtive alloc...");
    uint32_t freeBefore = SystemManagement::getKernelHeapManager()->getFreeMemory();
    primitiveAllocs();
    uint32_t freeAfter = SystemManagement::getKernelHeapManager()->getFreeMemory();

    *stream << "*** Primitive Allocs ***" << endl;
    *stream << "Small Allocs: " << stats[0] << " (Less then 1024 Bytes)" << endl;
    *stream << "Medium Allocs: " << stats[1] << " (Between 1024 and 4096 Bytes)" << endl;
    *stream << "Large Allocs: " << stats[2] << " (At least 4096 Bytes)\n" << endl;
    if(freeAfter - freeBefore != 0) {
        *stream << "Ups. Lost a few bytes. Leak: " << (freeAfter - freeBefore) << " Bytes" << endl;
         log.warn("Test failed - Found Memory-Leak in primitive alloc");
    } else {
         log.trace("Tested primitive alloc successfully");
    }

    stats[0] = 0;
    stats[1] = 0;
    stats[2] = 0;

     log.trace("Testing array alloc...");
    freeBefore = SystemManagement::getKernelHeapManager()->getFreeMemory();
    arrayAllocs();
    freeAfter = SystemManagement::getKernelHeapManager()->getFreeMemory();
    if(freeAfter - freeBefore != 0) {
        *stream << "Ups. Lost a few bytes. Leak: " << (freeAfter - freeBefore) << " Bytes" << endl;
        log.warn("Test failed - Found Memory-Leak in array alloc");
    } else {
         log.trace("Tested array alloc successfully");
    }
    *stream << "*** Array Allocs ***" << endl;
    *stream << "Small Allocs: " << stats[0] << " (Less then 50 Elements)" << endl;
    *stream << "Medium Allocs: " << stats[1] << " (Between 50 and 100 Elements)" << endl;
    *stream << "Large Allocs: " << stats[2] << " (At least 100 Elements)" << endl << endl;

     log.trace("Finished HeapTestApp");

    *stream << "Press [ENTER] to return" << endl;
    while (!kb->isKeyPressed(KeyEvent::RETURN));

    // cleanup
    Application::getInstance()->resume();
    Scheduler::getInstance()->exit();
}
