/**
 * HeapDemo
 *
 * A small demo to test the heap-memory allocator.
 *
 * @author Burak Akguel, Christian Gesse, Filip Krakowski, Fabian Ruhland, Michael Schoettner
 * @date 2018
 */

#include <kernel/services/InputService.h>
#include <kernel/services/TimeService.h>
#include <lib/libc/printf.h>
#include <kernel/services/GraphicsService.h>

#include "kernel/threads/Scheduler.h"
#include "user/HeapApp/HeapDemo.h"

#include "../../kernel/memory/SystemManagement.h"
#include "user/HeapApp/MyObj.h"
#include "lib/Byte.h"
#include "lib/Random.h"
#include "kernel/Kernel.h"

extern "C" {
    #include "lib/libc/stdlib.h"
}

/**
 * Runs the thread.
 */
void HeapDemo::run () {
	// get necessary services
    TextDriver *stream = (Kernel::getService<GraphicsService>())->getTextDriver();
    TimeService *timeService = Kernel::getService<TimeService>();
    Keyboard *kb = Kernel::getService<InputService>()->getKeyboard();

    // run the tests

#if HEAPDUMP
    *stream << endl << "  Please press <RETURN> for the initial heapdump" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;
#endif

    *stream << endl << "  Please press <RETURN> to start tests" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    stream->clear ();

    *stream << "Dynamically allocate 2 objects" << endl;
    *stream << "==============================" << endl << endl;

    *stream << "  Allocate objects" << endl;
    *stream << "  ================" << endl;
    MyObj *o = new MyObj(1,2);
    *stream << "    Allocated ";
    o->dump ();
    *stream << " at " << o << endl;

    MyObj *o2 = new MyObj(3,4);
    *stream << "    Allocated ";
    o->dump ();
    *stream << " at " << o2 << endl;

#if HEAPDUMP
    *stream << endl << "  Please press <RETURN> for heapdump" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;
#endif

    *stream << endl << "  Please press <RETURN to continue>" << endl;
    timeService->msleep(1000);
    while (!kb->isKeyPressed(28));

    *stream << endl;
    *stream << "Delete 2 objects" << endl;
    *stream << "================" << endl << endl;


    *stream << "  Deleting MyObj at " << o << endl;
    delete o;
    *stream << "  Deleting MyObj at " << o2 << endl;
    delete o2;

#if HEAPDUMP
    *stream << endl << "  Please press <RETURN> for heapdump" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;
#endif

    *stream << endl << "  Please press <RETURN> to continue" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));


    stream->clear ();
    *stream << "Allocate array of 3 objects and print the first one" << endl;
    *stream << "===================================================" << endl << endl;

    *stream << "  Allocating Array" << endl;
    *stream << "  ================" << endl;
    MyObj *pt = new MyObj[3];
    *stream << "    Allocated MyObj array " << " at " << pt << endl;

    pt[0].a=5;
    pt[0].b=6;

    *stream << endl;
    *stream << "    First object" << endl << "    ";
    o->dump ();
    *stream << " at " << o << endl;

#if HEAPDUMP
    *stream << endl << "  Please press <RETURN> for heapdump" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;
#endif

    *stream << endl << "  Please press <RETURN> to continue" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

	*stream << endl;
    *stream << "Release array memory" << endl;
    *stream << "====================" << endl << endl;

    *stream << "  Deleting MyObj array at " << pt << endl;
    delete[] pt;

#if HEAPDUMP
    *stream << endl << "  Please press <RETURN> for heapdump" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;
#endif

    *stream << endl << "  Please press <RETURN> to continue" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    stream->clear ();

    uint32_t *objects[128];

    printf("  Allocating objects randomly\n\n");

    Random *random = new Random(42, 0);

    // Randomly allocate objects
    unsigned int r;
    for (uint32_t *&object : objects) {
        r = random->rand(4);

        switch (r) {
            case 0:
                object = (unsigned int*) new MyObj(1,8);
                break;
            case 1:
                object = (unsigned int*) new MyObj[9][3];
                break;
            case 2:
                object = (unsigned int*) new char[32];
                break;
            case 3:
                object = (unsigned int*) new char[48];
                break;
            case 4:
                object = (unsigned int*) new Byte(0xDB);
                break;
            default:
                break;
        }
    }

#if HEAPDUMP
    *stream << endl << "  Please press <RETURN> for heapdump" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;
#endif

    *stream << endl << "  Please press <RETURN> to continue" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    // Shuffle allocated objects
    unsigned int src, dst;
    for (int i = 0; i < 10000; i++) {
        src = random->rand(128);
        dst = random->rand(128);

        unsigned int *tmp = objects[dst];
        objects[dst] = objects[src];
        objects[src] = tmp;
    }

    *stream << endl;
    *stream << "Delete randomly allocated objects." << endl;

    // Delete randomized objects
    for (auto &object : objects) {
        delete object;
    }

    delete random;

#if HEAPDUMP
    *stream << endl << "  Please press <RETURN> for heapdump" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;
#endif

    *stream << endl;
    *stream << "*** END OF DEMO ***" << endl;

    *stream << endl << "  Please press <RETURN> to continue" << endl;
	timeService->msleep(1000);
	while (!kb->isKeyPressed(28));
}
