#include "Zero.h"
#include "ZeroNode.h"
#include "kernel/Kernel.h"
#include "filesystem/FileSystem.h"

MODULE_PROVIDER {

    return new Zero();
};

int32_t Zero::initialize() {

    FileSystem *fileSystem = Kernel::getService<FileSystem>();

    fileSystem->addVirtualNode("/dev", new ZeroNode());

    return 0;
}

int32_t Zero::finalize() {

    // TODO
    //  Remove virtual node from FileSystem

    return 0;
}

String Zero::getName() {

    return "zero";
}

Util::Array<String> Zero::getDependencies() {

    return Util::Array<String>(0);
}
