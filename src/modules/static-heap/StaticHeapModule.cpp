#include "StaticHeapModule.h"
#include "StaticHeapMemoryManager.h"

MODULE_PROVIDER {

    return new StaticHeapModule();
};

int32_t StaticHeapModule::initialize() {

    StaticHeapMemoryManager staticHeapMemoryManager;

    MemoryManager::registerPrototype(&staticHeapMemoryManager);

    return 0;
}

int32_t StaticHeapModule::finalize() {

    MemoryManager::deregisterPrototype("StaticHeapMemoryManager");

    return 0;
}

String StaticHeapModule::getName() {

    return "static-heap";
}

Util::Array<String> StaticHeapModule::getDependencies() {

    return Util::Array<String>(0);
}