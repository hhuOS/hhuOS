#ifndef SystemInterface__include
#define SystemInterface__include

#include "stdint.h"
#include "MapInterface.h"

#define __ALLOC_KERNEL_MEM__(mem_service, type, size) \
    (type*)mem_service->allocateKernelMemory_c(mem_service, size, 0)

#define __ALLOC_KERNEL_MEM_ALIGN__(mem_service, type, size, alignment) \
    (type*)mem_service->allocateKernelMemory_c(mem_service, size, alignment)

#define __ALLOC_KERNEL_MEM_T__(mem_service, type, name, size) \
    type* name = (type*)mem_service->allocateKernelMemory_c(mem_service, \
    sizeof(type) * size, 0)

#define __ALLOC_KERNEL_MEM_S__(mem_service, type, name) \
    __ALLOC_KERNEL_MEM_T__(mem_service, type, name, 1)

#define __MAP_IO_KERNEL__(mem_service, type, size) \
    (type*)mem_service->mapIO(mem_service, size, 1)

#define __GET_PHYSICAL__(mem_service, virtual_addr) \
    __STRUCT_CALL__(mem_service, getPhysicalAddress, virtual_addr)

#define __GET_VIRTUAL__(mem_service, physical_addr, type) \
    (type*)__STRUCT_CALL__(mem_service, getVirtualAddress, physical_addr)

#define __ADD_VIRT__(mem_service, name, target_name) \
    uint32_t target_name = __PTR_TYPE__(uint32_t, \
    __GET_PHYSICAL__(mem_service, name)); \
    __STRUCT_CALL__(mem_service, addVirtualAddress, target_name, name)

#define __ADD_VIRT_TD__(mem_service, name, target_name) \
    uint32_t target_name = __PTR_TYPE__(uint32_t, \
    __GET_PHYSICAL__(mem_service, name)); \
    __STRUCT_CALL__(mem_service, addVirtualAddressTD, target_name, name)

#define __FREE_KERNEL_MEM__(mem_service, p) \
    mem_service->freeKernelMemory_c(mem_service, p, 0)

#define __MEM_SERVICE__(super_ptr, name) \
    MemoryService_C* name = \
        (MemoryService_C*)container_of(super_ptr, MemoryService_C, super)

#define __INT_SERVICE__(super_ptr, name) \
    InterruptService_C* name = \
      container_of(super_ptr, InterruptService_C, super)

enum Service_Type {
    MEMORY_TYPE = 1,
    INTERRUPT_TYPE = 2
};

typedef enum Service_Type Service_Type;

typedef void *SystemServiceP_C;

struct SystemService_C
{
    Service_Type (*type_of)(struct SystemService_C *c);

    void (*new_service)(struct SystemService_C *c, SystemServiceP_C service_pointer);

    SystemServiceP_C service_pointer;
};

typedef struct SystemService_C SystemService_C;

#define __INIT_MEMORY__(name, mem_pointer) \
    __ENTRY__(name, allocateKernelMemory_c) = &allocateKernelMemory_c; \
    __ENTRY__(name, reallocateKernelMemory_c) = &reallocateKernelMemory_c; \
    __ENTRY__(name, freeKernelMemory_c) = &freeKernelMemory_c; \
    __ENTRY__(name, allocateUserMemory_c) = &allocateUserMemory_c; \
    __ENTRY__(name, reallocateUserMemory_c) = &reallocateUserMemory_c; \
    __ENTRY__(name, freeUserMemory_c) = &freeUserMemory_c; \
    __ENTRY__(name, allocateLowerMemory_c) = &allocateLowerMemory_c; \
    __ENTRY__(name, reallocateLowerMemory_c) = &reallocateLowerMemory_c; \
    __ENTRY__(name, freeLowerMemory_c) = &freeLowerMemory_c; \
    __ENTRY__(name, mapPhysicalAddress) = &mapPhysicalAddress; \
    __ENTRY__(name, map) = &map; \
    __ENTRY__(name, mapIO_w_phy) = &mapIO_w_phy; \
    __ENTRY__(name, mapIO) = &mapIO; \
    __ENTRY__(name, unmap) = &unmap; \
    __ENTRY__(name, unmap_range) = &unmap_range; \
    __ENTRY__(name, getPhysicalAddress) = &getPhysicalAddress; \
    __ENTRY__(name, getVirtualAddress) = &getVirtualAddress; \
    __ENTRY__(name, addVirtualAddress) = &addVirtualAddress; \
    __ENTRY__(name, getVirtualAddressTD) = &getVirtualAddressTD; \
    __ENTRY__(name, addVirtualAddressTD) = &addVirtualAddressTD; \
    __ENTRY__(name, remove_virtualAddress) = &remove_virtualAddress; \
    __ENTRY__(name, remove_virtualAddressTD) = &remove_virtualAddressTD; \
    __ENTRY__(name, set_address_map) = &set_address_map; \
    \
    __SUPER__(name, type_of) = &type_of_memory; \
    __SUPER__(name, new_service) = &new_service; \
    \
    __CALL_SUPER__(name->super, new_service, mem_pointer)

struct MemoryService_C
{

    SystemService_C super;

    SuperMap* address_map;

    SuperMap* address_map_td;

    void *(*allocateKernelMemory_c)(struct MemoryService_C *m, uint32_t size, uint32_t aligment);

    void *(*reallocateKernelMemory_c)(struct MemoryService_C *m, void *pointer, uint32_t size, uint32_t alignment);

    void (*freeKernelMemory_c)(struct MemoryService_C *m, void *pointer, uint32_t alignment);

    void *(*allocateUserMemory_c)(struct MemoryService_C *m, uint32_t size, uint32_t alignment);

    void *(*reallocateUserMemory_c)(struct MemoryService_C *m, void *pointer, uint32_t size, uint32_t alignment);

    void (*freeUserMemory_c)(struct MemoryService_C *m, void *pointer, uint32_t alignment);

    void *(*allocateLowerMemory_c)(struct MemoryService_C *m, uint32_t size, uint32_t alignment);

    void *(*reallocateLowerMemory_c)(struct MemoryService_C *m, void *pointer, uint32_t size, uint32_t alignment);

    void (*freeLowerMemory_c)(struct MemoryService_C *m, void *pointer, uint32_t alignment);

    void (*mapPhysicalAddress)(struct MemoryService_C *m, uint32_t virtualAddress, uint32_t physicalAddress, uint16_t flags);

    void (*map)(struct MemoryService_C *m, uint32_t virtualAddress, uint16_t flags, int interrupt);

    void *(*mapIO_w_phy)(struct MemoryService_C *m, uint32_t physicalAddress, uint32_t size, int mapToKernelHeap);

    void *(*mapIO)(struct MemoryService_C *m, uint32_t size, int mapToKernelHeap);

    uint32_t (*unmap)(struct MemoryService_C *m, uint32_t virtualAddress);

    uint32_t (*unmap_range)(struct MemoryService_C *m, uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t breakCount);

    void *(*getPhysicalAddress)(struct MemoryService_C *m, void *virtualAddress);

    void* (*getVirtualAddress)(struct MemoryService_C* m, uint32_t physical_address);

    void (*addVirtualAddress)(struct MemoryService_C* m, uint32_t physical_address, void* qh);

    void (*set_address_map)(struct MemoryService_C* m);
    
    void* (*getVirtualAddressTD)(struct MemoryService_C* m, uint32_t physical_address);

    void (*addVirtualAddressTD)(struct MemoryService_C* m, uint32_t physical_address, void* td);

    void (*remove_virtualAddress)(struct MemoryService_C* m, uint32_t physical_address);

    void (*remove_virtualAddressTD)(struct MemoryService_C* m, uint32_t physical_address);
};

struct InterruptService_C{
    SystemService_C super;

    void (*new_interrupt_service)(struct InterruptService_C *interrupt_p);
    void (*add_interrupt_routine)(struct InterruptService_C *interrupt_c, uint8_t irq, 
                                  void* controller);

    void *usb_interrupt_handler;
};

typedef struct InterruptService_C InterruptService_C;
typedef struct MemoryService_C MemoryService_C;

#ifdef __cplusplus

extern "C"{
#endif

struct MemoryService_C *new_mem_service();

void new_interrupt_service(struct InterruptService_C *interrupt_c);

#ifdef __cplusplus
}
#endif

#endif