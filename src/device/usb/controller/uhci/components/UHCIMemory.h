#ifndef UHCI_MEMORY_INCLUDE
#define UHCI_MEMORY_INCLUDE

#include "../../../utility/Utils.h"
#include "stdint.h"
#include "UHCIRegister.h"
#include "../../../interfaces/IoPortInterface.h"

enum Memory_Type {
    IO,
    MEM
};

typedef enum Memory_Type Memory_Type;

struct Addr_Region{
    uint32_t size;
    Memory_Type (*get_mem_type)(struct Addr_Region* addr_region);
    size_t (*write)(struct Addr_Region* addr_region, void* b, uint8_t length, void* r);
    size_t (*read)(struct Addr_Region* addr_region, uint8_t length, void* r, void* buffer);
    void (*new_addr_region)(struct Addr_Region* addr_region, uint32_t size);
};

struct Memory_Region{
    struct Addr_Region super;
    void (*new_memory_region)(struct Memory_Region* mem_region, void* mem_start, uint32_t size);
    void* mem_start;
    void* mem_end;
};

struct IO_Region{
    struct Addr_Region super;
    void (*new_io_region)(struct IO_Region* io_region, IO_Port_Struct_C* io_port, uint32_t size);
    IO_Port_Struct_C* io_port;
};

void new_address_region(struct Addr_Region* addr_region, uint32_t size);
Memory_Type io_mem_type(struct Addr_Region* addr_region);
Memory_Type mem_mem_type(struct Addr_Region* addr_region);
void new_io_region(struct IO_Region* io_region, IO_Port_Struct_C* io_port, uint32_t size);
void new_mem_region(struct Memory_Region* mem_region, void* mem_start, uint32_t size);

size_t io_write(struct Addr_Region* addr_region, void* b, uint8_t length, void* r);
size_t io_read(struct Addr_Region* addr_region, uint8_t length, void* r, void* buffer);

size_t mem_write(struct Addr_Region* addr_region, void* b, uint8_t length, void* r);
size_t mem_read(struct Addr_Region* addr_region, uint8_t length, void* r, void* buffer);

typedef struct Addr_Region Addr_Region;
typedef struct Memory_Region Memory_Region;
typedef struct IO_Region IO_Region;

#endif