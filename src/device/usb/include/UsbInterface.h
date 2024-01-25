#ifndef USB_INTERFACE__INCLUDE
#define USB_INTERFACE__INCLUDE

#ifdef __cplusplus
extern "C"{
#endif    

void* interface_allocateMemory(uint32_t size, uint32_t alignment);

void* interface_reallocateMemory(void* p, uint32_t size, uint32_t alignment);

void interface_freeMemory(void* p, uint32_t alignment);

#ifdef __cplusplus
}
#endif

#endif