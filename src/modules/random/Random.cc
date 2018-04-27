#include "Random.h"
#include "RandomNode.h"
#include "kernel/Kernel.h"
#include "filesystem/FileSystem.h"

MODULE_PROVIDER {

    return new Random();
};

int32_t Random::initialize() {

    FileSystem *fileSystem = Kernel::getService<FileSystem>();

    fileSystem->addVirtualNode("/dev", new RandomNode());

    return 0;
}

int32_t Random::finalize() {

    // TODO
    //  Remove virtual node from FileSystem

    return 0;
}

String Random::getName() {

    return "random";
}

Util::Array<String> Random::getDependencies() {

    return Util::Array<String>(0);
}
