#include "kernel/core/Management.h"
#include "MemoryManagerDemo.h"
#include "MemoryManagerTest.h"

void MemoryManagerDemo::run() {

    TextDriver *stream = Kernel::System::getService<Kernel::GraphicsService>()->getTextDriver();

    MemoryManagerTest *tests[5] {
            new MemoryManagerTest(*Kernel::Management::getKernelHeapManager(), 1048576, 128, 8192, "KernelHeapMemoryManager"),
            new MemoryManagerTest(*Kernel::Management::getInstance().getIOMemoryManager(), 1048576, 128, 8192, "IOMemoryManager"),
            new MemoryManagerTest("FreeListMemoryManager", 1048576, 128, 8192),
            new MemoryManagerTest("BitmapMemoryManager", 1048576, 128, 128),
            new MemoryManagerTest("StaticHeapMemoryManager", 1048576, 128, 8192)
    };

    for(const auto test : tests) {
        stream->clear();
        test->execute();

        delete test;

        stream->clear();
    }
}
