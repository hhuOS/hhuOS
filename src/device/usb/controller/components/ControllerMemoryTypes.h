#ifndef CONTROLLER_MEMORY_TYPES_INCLUDE
#define CONTROLLER_MEMORY_TYPES_INCLUDE

#include "../../utility/Utils.h"
#include "stdint.h"
#include "../../interfaces/IoPortInterface.h"
#include "ControllerMemory.h"

#define __IO_write_BYTE__(data, value, reg) \
    reg->io_port->writeByte_IO_off_C(reg->io_port, type, value)
#define __IO_write_WORD__(data, value, reg) \
    reg->io_port->writeWord_IO_off_C(reg->io_port, type, value)
#define __IO_write_D_WORD__(data, value, reg) \
    reg->io_port->writeDoubleWord_IO_off_C(reg->io_port, type, \
                                               value)

#define __IO_write_value__(value, dtype, data) \
    value = *((__## dtype ## __ *)data)

#define __IO_write__(data, value, len, reg) \
    __IO_write_value__(value, len, data); \
    __IO_write_ ## len ## __(data, value, reg)

#define __IO_read_BYTE__(data, value, reg) \
    *(value) = reg->io_port->readByte_IO_off_C(reg->io_port, type)
#define __IO_read_WORD__(data, value, reg) \
    *(value) = \
        reg->io_port->readWord_IO_off_C(reg->io_port, type)
#define __IO_read_D_WORD__(data, value, reg) \
    *(value) = \
        reg->io_port->readDoubleWord_IO_off_C(reg->io_port, type)

#define __IO_read_value__(value, dtype, data) \
    value = (__## dtype ## __ *)data

#define __IO_read__(data, value, len, reg) \
    __IO_read_value__(value, len, data); \
    __IO_read_ ## len ## __(data, value, reg)

#define __DECLARE_IO_REGION__(io_region, size) \
    __SUPER__(io_region, new_addr_region) = &new_address_region; \
    __SUPER__(io_region, write) = &io_write; \
    __SUPER__(io_region, read) = &io_read; \
    __SUPER__(io_region, get_mem_type) = &io_mem_type; \
    \
    __CALL_SUPER__(io_region->super, new_addr_region, size)

#define __DECLARE_MEM_REGION__(mem_region, mem_start, size) // extend if should be supported

struct Memory_Region{
    struct Addr_Region super;
    void (*new_memory_region)(struct Memory_Region* mem_region, 
                                void* mem_start, uint32_t size);
    void* mem_start;
    void* mem_end;
};

struct IO_Region{
    struct Addr_Region super;
    void (*new_io_region)(struct IO_Region* io_region, 
                            IO_Port_Struct_C* io_port, uint32_t size);
    IO_Port_Struct_C* io_port;
};


void new_io_region(struct IO_Region* io_region, IO_Port_Struct_C* io_port, 
                    uint32_t size);
void new_mem_region(struct Memory_Region* mem_region, void* mem_start, 
                    uint32_t size);

typedef struct Memory_Region Memory_Region;
typedef struct IO_Region IO_Region;

#endif