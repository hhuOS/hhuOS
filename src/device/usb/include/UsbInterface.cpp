#include "stdint.h"
#include "UsbInterface.h"
#include "../../../lib/interface.h"

void* interface_allocateMemory(uint32_t size, uint32_t alignment){
    return allocateMemory(size, alignment);
}

void* interface_reallocateMemory(void* p, uint32_t size, uint32_t alignment){
    return reallocateMemory(p, size, alignment);
}

void interface_freeMemory(void* p, uint32_t alignment){
    freeMemory(p, alignment);
}
