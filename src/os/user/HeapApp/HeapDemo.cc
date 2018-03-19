/*****************************************************************************
 *                                                                           *
 *                            H E A P D E M O                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Demonstration der dynamischen Speicherverwaltung.        *
 *                                                                           *
 * Autor:           Michael Schoettner, HHU, 27.12.2016                      *
 *****************************************************************************/

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

/*****************************************************************************
 * Methode:         HeapDemo::run                                            *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Der Anwendungsthread.                                    *
 *****************************************************************************/
void HeapDemo::run () {
    TextDriver *stream = ((GraphicsService *) Kernel::getService(GraphicsService::SERVICE_NAME))->getTextDriver();
    TimeService *timeService = (TimeService*) Kernel::getService(TimeService::SERVICE_NAME);
    Keyboard *kb = ((InputService*)Kernel::getService(InputService::SERVICE_NAME))->getKeyboard();


    stream->clear ();
    *stream << "Dynamically allocate 2 objects" << endl;
    *stream << "==============================" << endl << endl;

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;

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

    SystemManagement::getKernelHeapManager()->dump();
    *stream << endl;

    *stream << endl << "  Please press <RETURN>" << endl;

    timeService->msleep(1000);

    while (!kb->isKeyPressed(28));

    //
    // zwei Objekte loeschen
    //
    stream->clear ();
    *stream << "Delete 2 objects" << endl;
    *stream << "================" << endl << endl;


    *stream << "  Deleting MyObj at " << o << endl;
    delete o;
    *stream << "  Deleting MyObj at " << o2 << endl;
    delete o2;

    *stream << endl;

    SystemManagement::getKernelHeapManager()->dump();

    *stream << endl << "  Please press <RETURN>" << endl;

    timeService->msleep(1000);

    while (!kb->isKeyPressed(28));


    //
    // Array mit drei Objekten anlegen
    //
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

    SystemManagement::getKernelHeapManager()->dump();

    *stream << endl << "  Please press <RETURN>" << endl;

    timeService->msleep(1000);

    while (!kb->isKeyPressed(28));


    //
    // Array freigeben
    //
    stream->clear ();
    *stream << "Release array memory" << endl;
    *stream << "====================" << endl << endl;

    *stream << "  Deleting MyObj array at " << pt << endl;
    delete pt;

    SystemManagement::getKernelHeapManager()->dump();

    *stream << endl << "  Please press <RETURN>" << endl;

    while (!kb->isKeyPressed(28));

    stream->clear ();
    *stream << "Memory test" << endl;
    *stream << "===========" << endl << endl;

    SystemManagement::getKernelHeapManager()->dump();

    unsigned int *objects[128];
    unsigned int alignments[] = {16, 32, 512, 4096};

    printf("  Allocating and deleting objects randomly\n\n");

    Random *random = new Random(42, 0);

    // Randomly allocate objects with random alignment
    unsigned int r, a, size;
    for(int i = 0; i < 128; i++) {
        r = random->rand(4);
//        a = random->rand(4);
//        size = random->rand(512) + 1;

        switch (r) {
            case 0:
                objects[i] = (unsigned int*) new MyObj(1,8);
                break;
            case 1:
                objects[i] = (unsigned int*) new MyObj[9][3];
                break;
            case 2:
                objects[i] = (unsigned int*) new char[32];
                break;
            case 3:
                objects[i] = (unsigned int*) new char[48];
                break;
            case 4:
                objects[i] = (unsigned int*) new Byte(0xDB);
                break;
        }
    }

    // Shuffle allocated objects
    unsigned int src, dst;
    for (int i = 0; i < 10000; i++) {
        src = random->rand(128);
        dst = random->rand(128);

        unsigned int *tmp = objects[dst];
        objects[dst] = objects[src];
        objects[src] = tmp;
    }

    // Delete randomized objects
    for (int i = 0; i < 128; i++) {
        delete objects[i];
    }

    delete random;

    SystemManagement::getKernelHeapManager()->dump();

    *stream << endl;
    *stream << "*** END OF DEMO ***" << endl;

    // selbst terminieren
    Scheduler::getInstance()->exit();
}
