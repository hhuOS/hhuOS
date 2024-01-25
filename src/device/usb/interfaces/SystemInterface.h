#ifndef SystemInterface__include
#define SystemInterface__include

#include "stdint.h"
#include "MapInterface.h"

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
    void (*add_interrupt_routine)(struct InterruptService_C *interrupt_c, uint8_t irq, void (*handler_function)(void* controller), void* controller);

    void *usb_interrupt_handler;
};

typedef struct InterruptService_C InterruptService_C;
typedef struct MemoryService_C MemoryService_C;

#ifdef __cplusplus

extern "C"{
#endif

// Memory Service

void *allocateKernelMemory_c(MemoryService_C *m, uint32_t size, uint32_t alignment);

void *reallocateKernelMemory_c(MemoryService_C *m, void *pointer, uint32_t size, uint32_t alignment);

void freeKernelMemory_c(MemoryService_C *m, void *pointer, uint32_t alignment);

void *allocateUserMemory_c(MemoryService_C *m, uint32_t size, uint32_t alignment);

void *reallocateUserMemory_c(MemoryService_C *m, void *pointer, uint32_t size, uint32_t alignment);

void freeUserMemory_c(MemoryService_C *m, void *pointer, uint32_t alignment);

void *allocateLowerMemory_c(MemoryService_C *m, uint32_t size, uint32_t alignment);

void *reallocateLowerMemory_c(MemoryService_C *m, void *pointer, uint32_t size, uint32_t alignment);

void freeLowerMemory_c(MemoryService_C *m, void *pointer, uint32_t alignment);

void mapPhysicalAddress(MemoryService_C *m, uint32_t virtualAddress, uint32_t physicalAddress, uint16_t flags);

void map(MemoryService_C *m, uint32_t virtualAddress, uint16_t flags, int interrupt);

void *mapIO_w_phy(MemoryService_C *m, uint32_t physicalAddress, uint32_t size, int mapToKernelHeap);

void *mapIO(MemoryService_C *m, uint32_t size, int mapToKernelHeap);

uint32_t unmap(MemoryService_C *m, uint32_t virtualAddress);

uint32_t unmap_range(MemoryService_C *m, uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t breakCount);

void* getPhysicalAddress(MemoryService_C *m, void *virtualAddress);

void* getVirtualAddress(MemoryService_C* m, uint32_t physical_address);

void addVirtualAddress(MemoryService_C* m, uint32_t physical_address, void* qh);

void set_address_map(MemoryService_C* m);

void* getVirtualAddressTD(MemoryService_C* m, uint32_t physical_address);

void addVirtualAddressTD(MemoryService_C* m, uint32_t physical_address, void* td);

void remove_virtualAddress(MemoryService_C* m, uint32_t physical_address);

void remove_virtualAddressTD(MemoryService_C* m, uint32_t physical_address);

void new_service(struct SystemService_C *service_c, SystemServiceP_C service_p);

struct MemoryService_C *new_mem_service();

void new_interrupt_service(struct InterruptService_C *interrupt_c);

void add_interrupt_routine(struct InterruptService_C *interrupt_c, uint8_t irq, void (*handler_function)(void* controller), void* controller);

Service_Type type_of_interrupt(SystemService_C *c);
Service_Type type_of_memory(SystemService_C *c);

#ifdef __cplusplus
}
#endif

#endif