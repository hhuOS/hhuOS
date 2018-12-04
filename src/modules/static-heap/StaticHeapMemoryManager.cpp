#include <lib/memory/MemoryUtil.h>
#include "StaticHeapMemoryManager.h"

StaticHeapMemoryManager::StaticHeapMemoryManager() : MemoryManager(),
        currentPosition(memoryStartAddress) {

}

StaticHeapMemoryManager::StaticHeapMemoryManager(const StaticHeapMemoryManager &copy) : StaticHeapMemoryManager() {

}

void StaticHeapMemoryManager::init(uint32_t memoryStartAddress, uint32_t memoryEndAddress, bool doUnmap) {
    MemoryManager::init(memoryStartAddress, memoryEndAddress, false);

    freeMemory = memoryEndAddress - memoryStartAddress;
}

String StaticHeapMemoryManager::getName() {
    return NAME;
}

void *StaticHeapMemoryManager::alloc(uint32_t size) {
    return alloc(size, 0);
}

void *StaticHeapMemoryManager::alloc(uint32_t size, uint32_t alignment) {
    if(freeMemory < size) {
        return nullptr;
    }

    lock.acquire();

    currentPosition = MemoryUtil::alignUp(currentPosition, alignment);

    void *ret = reinterpret_cast<void *>(currentPosition);

    currentPosition += size;
    freeMemory -= size;

    lock.release();

    return ret;
}

void *StaticHeapMemoryManager::realloc(void *ptr, uint32_t size) {
    return realloc(ptr, size, 0);
}

void *StaticHeapMemoryManager::realloc(void *ptr, uint32_t size, uint32_t alignment) {
    void *ret = alloc(size, alignment);

    if(ret == nullptr) {
        return ret;
    }

    memcpy(ret, ptr, size);

    return ret;
}

void StaticHeapMemoryManager::free(void *ptr) {
    free(ptr, 0);
}

void StaticHeapMemoryManager::free(void *ptr, uint32_t alignment) {

}

void StaticHeapMemoryManager::dump() {
    printf("  StaticHeapMemoryManager: Current position = %08x\n", currentPosition);
}