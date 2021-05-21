#include "HeapMemoryManager.h"

namespace Kernel {

void HeapMemoryManager::initialize(uint32_t startAddress, uint32_t endAddress) {
    memoryStartAddress = startAddress;
    memoryEndAddress = endAddress;
    freeMemory = endAddress - startAddress;
}

void *HeapMemoryManager::allignedAlloc(uint32_t size, uint32_t alignment) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"MemoryManager: Operation 'aligned allignedAlloc' is not supported!");
}

void *HeapMemoryManager::realloc(void *ptr, uint32_t size) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"MemoryManager: Operation 'realloc' is not supported!");
}

void *HeapMemoryManager::realloc(void *ptr, uint32_t size, uint32_t alignment) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"MemoryManager: Operation 'aligned realloc' is not supported!");
}

void HeapMemoryManager::alignedFree(void *ptr, uint32_t alignment) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"MemoryManager: Operation 'aligned free' is not supported!");
}

}