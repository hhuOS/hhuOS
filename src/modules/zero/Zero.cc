#include "kernel/filesystem/RamFs/RamFsDriver.h"
#include "kernel/services/FileSystem.h"
#include "ZeroNode.h"

extern "C" {
    int module_init();
    int module_fini();
}

int module_init() {

    FileSystem *fileSystem = (FileSystem*) Kernel::getService(FileSystem::SERVICE_NAME);

    return fileSystem->addVirtualNode("/dev", new ZeroNode());
}

int module_fini() {
    // TODO
    //  unmount zero node
    return 0;
}
