#include "kernel/filesystem/RamFs/DevFs/DevFsDriver.h"
#include "kernel/services/FileSystem.h"
#include "Random.h"
#include "RandomNode.h"

extern "C" {
    int module_init();
    int module_fini();
}

int module_init() {
    FileSystem *fileSystem = (FileSystem*) Kernel::getService(FileSystem::SERVICE_NAME);

    char *pathInMount;

    FileSystem::MountPoint *mountPoint = fileSystem->getMountInfo("/dev", pathInMount);

    DevFsDriver *driver = (DevFsDriver*) mountPoint->driver;

    driver->mount(new RandomNode());

    return 0;
}

int module_fini() {
    // TODO
    //  unmount zero node
    return 0;
}
