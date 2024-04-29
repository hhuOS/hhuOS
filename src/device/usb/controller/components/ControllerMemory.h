#ifndef CONTROLLER_MEMORY_INCLUDE
#define CONTROLLER_MEMORY_INCLUDE

#include "stdint.h"
#include "../../utility/Utils.h"

enum _WR_SIZE_{
    BYTE = 0x01,
    WORD = 0x02,
    D_WORD = 0x04
};

enum Memory_Type {
    IO,
    MEM
};

typedef enum Memory_Type Memory_Type;
typedef enum _WR_SIZE_ _WR_SIZE_;

struct Addr_Region{
    uint32_t size;
    Memory_Type (*get_mem_type)(struct Addr_Region* addr_region);
    size_t (*write)(struct Addr_Region* addr_region, void* b, uint8_t length, void* r);
    size_t (*read)(struct Addr_Region* addr_region, uint8_t length, void* r, void* buffer);
    void (*new_addr_region)(struct Addr_Region* addr_region, uint32_t size);
};

typedef struct Addr_Region Addr_Region;

static inline void new_address_region(struct Addr_Region *addr_region, uint32_t size) {
  addr_region->size = size;
}

#endif