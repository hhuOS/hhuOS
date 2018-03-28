#include <lib/Random.h>
#include "MemoryTest.h"

#include "../../kernel/memory/SystemManagement.h"
#include "MyObj.h"


extern "C" {
    #include <lib/libc/stdlib.h>
}

Spinlock MemoryTest::printLock;

void MemoryTest::run() {

    unsigned int **objects = new unsigned int*[128];
    unsigned int alignments[] = {16, 32, 512, 4096};

    Random *random = new Random(42, 0);

    // Randomly allocate objects with random alignment
    unsigned int r, a, size;
    for(int i = 0; i < 128; i++) {
        r = random->rand(4);
        a = random->rand(4);
        size = random->rand(512) + 1;

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
                objects[i] = (unsigned int*) aligned_alloc(alignments[a], size);
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
    delete objects;

    printLock.acquire();

    SystemManagement::getKernelHeapManager()->dump();

    printLock.release();

    while(1);
}
