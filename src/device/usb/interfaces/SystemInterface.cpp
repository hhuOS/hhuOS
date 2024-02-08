#include "SystemInterface.h"
#include "../../../kernel/service/MemoryService.h"
#include "../../../kernel/service/InterruptService.h"
#include "../../../kernel/system/System.h"
#include "../controller/UsbInterruptHandler.h"
#include "MapInterface.h"

extern "C"{
#include "../controller/UsbController.h"
}

void new_service(SystemService_C* service_c, SystemServiceP_C service_p){
    service_c->service_pointer = service_p;
}

MemoryService_C* new_mem_service(){
    Kernel::MemoryService& mem_service = Kernel::System::getService<Kernel::MemoryService>();
    Kernel::MemoryService* s = &mem_service;

    MemoryService_C* mem_c = (MemoryService_C*)mem_service.allocateKernelMemory(sizeof(MemoryService_C));

    mem_c->allocateKernelMemory_c = &allocateKernelMemory_c;
    mem_c->reallocateKernelMemory_c = &reallocateKernelMemory_c;
    mem_c->freeKernelMemory_c = &freeKernelMemory_c;
    mem_c->allocateUserMemory_c = &allocateUserMemory_c;
    mem_c->reallocateUserMemory_c = &reallocateUserMemory_c;
    mem_c->freeUserMemory_c = &freeUserMemory_c;
    mem_c->allocateLowerMemory_c = &allocateLowerMemory_c;
    mem_c->reallocateLowerMemory_c = &reallocateLowerMemory_c;
    mem_c->freeLowerMemory_c = &freeLowerMemory_c;
    mem_c->mapPhysicalAddress = &mapPhysicalAddress;
    mem_c->map = &map;
    mem_c->mapIO_w_phy = &mapIO_w_phy;
    mem_c->mapIO = &mapIO;
    mem_c->unmap = &unmap;
    mem_c->unmap_range = &unmap_range;
    mem_c->getPhysicalAddress = &getPhysicalAddress;
    mem_c->getVirtualAddress = &getVirtualAddress;
    mem_c->addVirtualAddress = &addVirtualAddress;
    mem_c->getVirtualAddressTD = &getVirtualAddressTD;
    mem_c->addVirtualAddressTD = &addVirtualAddressTD;
    mem_c->remove_virtualAddress = &remove_virtualAddress;
    mem_c->remove_virtualAddressTD = &remove_virtualAddressTD;
    mem_c->set_address_map = &set_address_map;

    mem_c->super.type_of = &type_of_memory;
    mem_c->super.new_service = &new_service;
    mem_c->super.new_service(&mem_c->super, s);

    mem_c->set_address_map(mem_c);

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
    Kernel::InterruptService& interrupt_service = Kernel::System::getService<Kernel::InterruptService>();
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

void* allocateLowerMemory_c(MemoryService_C* m, uint32_t size, uint32_t alignment){
    return ((Kernel::MemoryService*)m->super.service_pointer)->allocateLowerMemory(size,alignment);
}

void* reallocateLowerMemory_c(MemoryService_C* m, void* pointer, uint32_t size, uint32_t alignment){
    return ((Kernel::MemoryService*)m->super.service_pointer)->reallocateLowerMemory(pointer,size,alignment);
}

void freeLowerMemory_c(MemoryService_C* m, void* pointer, uint32_t alignment){
    ((Kernel::MemoryService*)m->super.service_pointer)->freeLowerMemory(pointer,alignment);
}

void mapPhysicalAddress(MemoryService_C* m, uint32_t virtualAddress, uint32_t physicalAddress, uint16_t flags){
    ((Kernel::MemoryService*)m->super.service_pointer)->mapPhysicalAddress(virtualAddress,physicalAddress,flags);
}

void map(MemoryService_C* m, uint32_t virtualAddress, uint16_t flags, int interrupt){
    ((Kernel::MemoryService*)m->super.service_pointer)->map(virtualAddress,flags, (bool)interrupt);
}

void *mapIO_w_phy(MemoryService_C* m, uint32_t physicalAddress, uint32_t size, int mapToKernelHeap){
    return ((Kernel::MemoryService*)m->super.service_pointer)->mapIO(physicalAddress,size,(bool)mapToKernelHeap);
}

void *mapIO(MemoryService_C* m, uint32_t size, int mapToKernelHeap){
    return ((Kernel::MemoryService*)m->super.service_pointer)->mapIO(size,(bool)mapToKernelHeap);
}

uint32_t unmap(MemoryService_C* m, uint32_t virtualAddress){
    return ((Kernel::MemoryService*)m->super.service_pointer)->unmap(virtualAddress);
}

uint32_t unmap_range(MemoryService_C* m, uint32_t virtualStartAddress, uint32_t virtualEndAddress, uint32_t breakCount){
    return ((Kernel::MemoryService*)m->super.service_pointer)->unmap(virtualStartAddress,virtualEndAddress,breakCount);
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