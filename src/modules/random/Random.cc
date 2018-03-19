#include "kernel/services/FileSystem.h"
#include "RandomNode.h"

extern "C" {
    int module_init();
    int module_fini();
}

int32_t module_init() {

    FileSystem *fileSystem = (FileSystem*) Kernel::getService(FileSystem::SERVICE_NAME);

    fileSystem->addVirtualNode("/dev", new RandomNode());

    return 0;
}

int32_t module_fini() {
    // TODO
    //  unmount zero node
    return 0;
}
