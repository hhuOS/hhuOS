#include "UHCIMemory.h"
#include "UHCIRegister.h"
#include "stdint.h"

void new_io_region(struct IO_Region* io_region, IO_Port_Struct_C* io_port, uint32_t size){
    io_region->io_port = io_port;
    io_region->super.new_addr_region = &new_address_region;
    io_region->super.write = &io_write;
    io_region->super.read = &io_read;
    io_region->super.get_mem_type = &io_mem_type;

    io_region->super.new_addr_region(&io_region->super, size);
}

void new_mem_region(struct Memory_Region* mem_region, void* mem_start, uint32_t size){
    mem_region->mem_start = mem_start;
    mem_region->mem_end   = mem_start + size;
    mem_region->super.new_addr_region = &new_address_region;
    mem_region->super.get_mem_type = &mem_mem_type;
    // if using memIO write a function for read and write
}

Memory_Type mem_mem_type(struct Addr_Region* addr_region){
    return MEM;
}

Memory_Type io_mem_type(struct Addr_Region* addr_region){
    return IO;
}

void new_address_region(struct Addr_Region* addr_region, uint32_t size){
    addr_region->size = size;
}

size_t io_write(struct Addr_Region* addr_region, void* b, uint8_t length, void* r){
    uint8_t int8_tvalue;
    uint16_t int16_tvalue;
    uint32_t int32_tvalue;

    IO_Region* i_o_reg = container_of(addr_region,IO_Region,super);

    enum Register_Type type = *((enum Register_Type*)r);

    if(length == 0x01){
        int8_tvalue = *((uint8_t*)b);
        i_o_reg->io_port->writeByte_IO_off_C(i_o_reg->io_port, type, int8_tvalue);
        return length;
    }
    else if(length == 0x02){
        int16_tvalue = *((uint16_t*)b);
        i_o_reg->io_port->writeWord_IO_off_C(i_o_reg->io_port, type, int16_tvalue);
        return length;
    }
    else if(length == 0x04){
        int32_tvalue = *((uint32_t*)b);
        i_o_reg->io_port->writeDoubleWord_IO_off_C(i_o_reg->io_port, type, int32_tvalue);
        return length;
    }
    return 0;
}

size_t io_read(struct Addr_Region* addr_region, uint8_t length, void* r, void* buffer){
    uint8_t* int8_pvalue;
    uint16_t* int16_pvalue;
    uint32_t* int32_pvalue;

    if(buffer == (void*)0) return 0;

    enum Register_Type type = *((enum Register_Type*)r);

    IO_Region* i_o_reg = (IO_Region*)container_of(addr_region, IO_Region, super);

    if(length == 0x01){
        int8_pvalue = (uint8_t*)buffer;
        *(int8_pvalue) = i_o_reg->io_port->readByte_IO_off_C(i_o_reg->io_port,type);
        return length;
    }
    else if(length == 0x02){
        int16_pvalue = (uint16_t*)buffer;
        *(int16_pvalue) = i_o_reg->io_port->readWord_IO_off_C(i_o_reg->io_port,type);
        return length;
    }
    else if(length == 0x04){
        int32_pvalue = (uint32_t*)buffer;
        *(int32_pvalue) = i_o_reg->io_port->readDoubleWord_IO_off_C(i_o_reg->io_port,type);
        return length;
    }
    return 0;
}

size_t mem_write(struct Addr_Region* addr_region, void* b, uint8_t length, void* r){
    return 0;
}

size_t mem_read(struct Addr_Region* addr_region, uint8_t length, void* r, void* buffer){
    return 0;
}