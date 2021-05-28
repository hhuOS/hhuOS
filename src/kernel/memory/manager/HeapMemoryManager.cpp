#include "HeapMemoryManager.h"

namespace Kernel {

void HeapMemoryManager::initialize(uint32_t startAddress, uint32_t endAddress) {
    memoryStartAddress = startAddress;
    memoryEndAddress = endAddress;
}

void *HeapMemoryManager::alloc(uint32_t size) {
    return alignedAlloc(size, 0);
}

void* HeapMemoryManager::realloc(void *ptr, uint32_t size) {
    return alignedRealloc(ptr, size, 0);
}

void HeapMemoryManager::free(void *ptr) {
    alignedFree(ptr, 0);
}

void* HeapMemoryManager::alignedAlloc(uint32_t size, uint32_t alignment) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"MemoryManager: Operation 'aligned alignedAlloc' is not supported!");
}

void *HeapMemoryManager::alignedRealloc(void *ptr, uint32_t size, uint32_t alignment) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"MemoryManager: Operation 'aligned alignedRealloc' is not supported!");
}

void HeapMemoryManager::alignedFree(void *ptr, uint32_t alignment) {
    Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"MemoryManager: Operation 'aligned free' is not supported!");
}

uint32_t HeapMemoryManager::getStartAddress() const {
    return memoryStartAddress;
}

uint32_t HeapMemoryManager::getEndAddress() const {
    return memoryEndAddress;
}

uint32_t HeapMemoryManager::getFreeMemory() const {
    return memoryEndAddress - memoryStartAddress;
}

}