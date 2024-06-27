#include "SystemInterface.h"
#include "../../../kernel/service/MemoryService.h"
#include "../../../kernel/service/InterruptService.h"
#include "../../../kernel/service/Service.h"
#include "../controller/UsbInterruptHandler.h"
#include "MapInterface.h"

extern "C"{
#include "../controller/UsbController.h"
static void add_interrupt_routine(struct InterruptService_C *interrupt_c, uint8_t irq, void* controller);
static Service_Type type_of_interrupt(SystemService_C *c);
static Service_Type type_of_memory(SystemService_C *c);
static void *allocateKernelMemory_c(MemoryService_C *m, uint32_t size, uint32_t alignment);
static void *reallocateKernelMemory_c(MemoryService_C *m, void *pointer, uint32_t size, uint32_t alignment);
static void freeKernelMemory_c(MemoryService_C *m, void *pointer, uint32_t alignment);
static void *allocateUserMemory_c(MemoryService_C *m, uint32_t size, uint32_t alignment);
static void *reallocateUserMemory_c(MemoryService_C *m, void *pointer, uint32_t size, uint32_t alignment);
static void freeUserMemory_c(MemoryService_C *m, void *pointer, uint32_t alignment);
static void mapPhysicalAddress(MemoryService_C *m, void* physicalAddress, void* virtualAddress, 
    uint32_t pageCount, uint16_t flags);
static void map(MemoryService_C *m, void* virtualAddress, uint32_t pageCount, uint16_t flags);
static void *mapIO_w_phy(MemoryService_C *m, void* physicalAddress, uint32_t pageCount, 
        int mapToKernelHeap);
static void *mapIO(MemoryService_C *m, uint32_t pageCount, int mapToKernelHeap);
static void* unmap(MemoryService_C *m, void* virtualAddress, uint32_t pageCount);
static void* getPhysicalAddress(MemoryService_C *m, void *virtualAddress);
static void* getVirtualAddress(MemoryService_C* m, uint32_t physical_address);
static void addVirtualAddress(MemoryService_C* m, uint32_t physical_address, void* qh);
static void set_address_map(MemoryService_C* m);
static void* getVirtualAddressTD(MemoryService_C* m, uint32_t physical_address);
static void addVirtualAddressTD(MemoryService_C* m, uint32_t physical_address, void* td);
static void remove_virtualAddress(MemoryService_C* m, uint32_t physical_address);
static void remove_virtualAddressTD(MemoryService_C* m, uint32_t physical_address);
}

static inline void new_service(SystemService_C* service_c, SystemServiceP_C service_p){
    service_c->service_pointer = service_p;
}

MemoryService_C* new_mem_service(){
    Kernel::MemoryService& mem_service = Kernel::Service::getService<Kernel::MemoryService>();
    Kernel::MemoryService* s = &mem_service;

    MemoryService_C* mem_c = (MemoryService_C*)mem_service.allocateKernelMemory(sizeof(MemoryService_C));

    __INIT_MEMORY__(mem_c, s);
    __STRUCT_CALL__(mem_c, set_address_map);

    return mem_c;
}

void set_address_map(MemoryService_C* mem_service){
    Address_Map* a_map = (Address_Map*)mem_service->allocateKernelMemory_c(mem_service, sizeof(Address_Map), 0);
    Address_TD_Map* td_map = (Address_TD_Map*)mem_service->allocateKernelMemory_c(mem_service, sizeof(Address_TD_Map), 0);

    a_map->new_map = &newAddressMap;
    a_map->new_map(a_map, "Map<uint32_t,QH*>");

    td_map->new_map = &newAddressTDMap;
    td_map->new_map(td_map, "Map<uint32_t,TD*>");

    mem_service->address_map = (SuperMap*)a_map;
    mem_service->address_map_td = (SuperMap*)td_map;
}

void new_interrupt_service(InterruptService_C* interrupt_c){
    Kernel::InterruptService& interrupt_service = Kernel::Service::getService<Kernel::InterruptService>();
    Kernel::InterruptService* i = &interrupt_service;

    interrupt_c->usb_interrupt_handler = 0;
    interrupt_c->add_interrupt_routine = &add_interrupt_routine;
    interrupt_c->super.new_service = &new_service;
    interrupt_c->super.type_of = &type_of_interrupt;
    interrupt_c->super.new_service(&interrupt_c->super,i);
}

Service_Type type_of_interrupt(SystemService_C* c){
    return INTERRUPT_TYPE;
}

Service_Type type_of_memory(SystemService_C* c){
    return MEMORY_TYPE;
}

void add_interrupt_routine(InterruptService_C* interrupt_c, uint8_t irq, void* controller){
    Device::Usb::UsbInterruptHandler* handler = new Device::Usb::UsbInterruptHandler(irq, (UsbController*)controller);
    handler->plugin();

    interrupt_c->usb_interrupt_handler = (void*)handler;
}

void* allocateKernelMemory_c(MemoryService_C* m, uint32_t size, uint32_t alignment){
    return ((Kernel::MemoryService*)m->super.service_pointer)->allocateKernelMemory(size,alignment);
}

void* reallocateKernelMemory_c(MemoryService_C* m, void* pointer, uint32_t size, uint32_t alignment){
   return ((Kernel::MemoryService*)m->super.service_pointer)->reallocateKernelMemory(pointer,size,alignment);
}

void freeKernelMemory_c(MemoryService_C* m, void* pointer, uint32_t alignment){
    ((Kernel::MemoryService*)m->super.service_pointer)->freeKernelMemory(pointer,alignment);
}

void* allocateUserMemory_c(MemoryService_C* m, uint32_t size, uint32_t alignment){
    return ((Kernel::MemoryService*)m->super.service_pointer)->allocateUserMemory(size,alignment);
}

void* reallocateUserMemory_c(MemoryService_C* m, void* pointer, uint32_t size, uint32_t alignment){
    return ((Kernel::MemoryService*)m->super.service_pointer)->reallocateUserMemory(pointer,size,alignment);
}

void freeUserMemory_c(MemoryService_C* m, void* pointer, uint32_t alignment){
    ((Kernel::MemoryService*)m->super.service_pointer)->freeUserMemory(pointer,alignment);
}

void mapPhysicalAddress(MemoryService_C* m, void* physicalAddress, void* virtualAddress, uint32_t pageCount, 
        uint16_t flags){
    ((Kernel::MemoryService*)m->super.service_pointer)->mapPhysical(physicalAddress, virtualAddress, pageCount, flags);
}

void map(MemoryService_C* m, void* virtualAddress, uint32_t pageCount, uint16_t flags){
    ((Kernel::MemoryService*)m->super.service_pointer)->map(virtualAddress,pageCount, flags);
}

void *mapIO_w_phy(MemoryService_C* m, void* physicalAddress, uint32_t pageCount, 
        int mapToKernelHeap){
    return ((Kernel::MemoryService*)m->super.service_pointer)->mapIO(physicalAddress,pageCount,(bool)mapToKernelHeap);
}

void *mapIO(MemoryService_C* m, uint32_t pageCount, int mapToKernelHeap){
    return ((Kernel::MemoryService*)m->super.service_pointer)->mapIO(pageCount,(bool)mapToKernelHeap);
}

void* unmap(MemoryService_C* m, void* virtualAddress, uint32_t pageCount){
    return ((Kernel::MemoryService*)m->super.service_pointer)->unmap(virtualAddress, pageCount);
}

void* getPhysicalAddress(MemoryService_C* m, void *virtualAddress){
    return ((Kernel::MemoryService*)m->super.service_pointer)->getPhysicalAddress(virtualAddress);
}

void* getVirtualAddress(MemoryService_C* m, uint32_t physical_address){
    SuperMap* a_map = m->address_map;
    
    if(a_map->contains_c(a_map, &physical_address)){
        return a_map->get_c(a_map, &physical_address);
    }
    return (void*)0;
}

void addVirtualAddress(MemoryService_C* m, uint32_t physical_address, void* qh){
    SuperMap* a_map = m->address_map;

    if(!a_map->contains_c(a_map, &physical_address)){
        a_map->put_c(a_map, &physical_address, qh);
    }
}

void* getVirtualAddressTD(MemoryService_C* m, uint32_t physical_address){
    SuperMap* a_map = m->address_map_td;

    if(a_map->contains_c(a_map, &physical_address)){
        return a_map->get_c(a_map, &physical_address);
    }
    return (void*)0;
}

void addVirtualAddressTD(MemoryService_C* m, uint32_t physical_address, void* td){
    SuperMap* a_map = m->address_map_td;

    if(!a_map->contains_c(a_map, &physical_address)){
        a_map->put_c(a_map, &physical_address, td);
    }
}

void remove_virtualAddress(MemoryService_C* m, uint32_t physical_address){
    SuperMap* a_map = m->address_map;

    if(a_map->contains_c(a_map, &physical_address)){
        a_map->remove_c(a_map, &physical_address);
    }
}   

void remove_virtualAddressTD(MemoryService_C* m, uint32_t physical_address){
    SuperMap* a_map = m->address_map_td;

    if(a_map->contains_c(a_map, &physical_address)){
        a_map->remove_c(a_map, &physical_address);
    }
}