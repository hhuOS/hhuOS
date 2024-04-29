#include "stdint.h"
#include "ControllerMemoryTypes.h"
#include "ControllerRegister.h"
#include "ControllerMemory.h"

void new_io_region(struct IO_Region *io_region, IO_Port_Struct_C *io_port,
                   uint32_t size) {
  __DECLARE_IO_REGION__(io_region, io_port, size);
}

void new_mem_region(struct Memory_Region *mem_region, void *mem_start,
                    uint32_t size) {
  mem_region->mem_start = mem_start;
  mem_region->mem_end = mem_start + size;
  mem_region->super.new_addr_region = &new_address_region;
  mem_region->super.get_mem_type = &mem_mem_type;
  // if using memIO write a function for read and write
}

Memory_Type mem_mem_type(struct Addr_Region *addr_region) { return MEM; }

Memory_Type io_mem_type(struct Addr_Region *addr_region) { return IO; }



size_t io_write(struct Addr_Region *addr_region, void *b, uint8_t length,
                void *r) {
  uint8_t int8_tvalue;
  uint16_t int16_tvalue;
  uint32_t int32_tvalue;

  IO_Region *i_o_reg = container_of(addr_region, IO_Region, super);

  enum Register_Type type = *((enum Register_Type *)r);

  if (length == BYTE) {;
    __IO_write__(b, int8_tvalue, BYTE, i_o_reg);
  } else if (length == WORD) {
    __IO_write__(b, int16_tvalue, WORD, i_o_reg);
  } else if (length == D_WORD) {
    __IO_write__(b, int32_tvalue, D_WORD, i_o_reg);
  } else length = 0;

  return length;
}

size_t io_read(struct Addr_Region *addr_region, uint8_t length, void *r,
               void *buffer) {
  uint8_t *int8_pvalue;
  uint16_t *int16_pvalue;
  uint32_t *int32_pvalue;

  if (buffer == (void *)0)
    return 0;

  enum Register_Type type = *((enum Register_Type *)r);

  IO_Region *i_o_reg = (IO_Region *)container_of(addr_region, IO_Region, super);

  if (length == BYTE) {
    __IO_read__(buffer, int8_pvalue, BYTE, i_o_reg);
  } else if (length == WORD) {
    __IO_read__(buffer, int16_pvalue, WORD, i_o_reg);
  } else if (length == D_WORD) {
    __IO_read__(buffer, int32_pvalue, D_WORD, i_o_reg);
  } else length = 0;
  return length;
}

size_t mem_write(struct Addr_Region *addr_region, void *b, uint8_t length,
                 void *r) {
  return 0;
}

size_t mem_read(struct Addr_Region *addr_region, uint8_t length, void *r,
                void *buffer) {
  return 0;
}