#ifndef IO_Port__include
#define IO_Port__include

typedef void* IO_Port_C;

#include "stdint.h"

struct IO_Port_Struct_C{
    void (*newIO_Port)(struct IO_Port_Struct_C* port_struct, uint16_t address);

    void (*writeByte_IO_C)(struct IO_Port_Struct_C* p, uint8_t value);

    void (*writeByte_IO_off_C)(struct IO_Port_Struct_C* p, uint16_t offset, uint8_t value);

    void (*writeWord_IO_C)(struct IO_Port_Struct_C* p, uint16_t value);

    void (*writeWord_IO_off_C)(struct IO_Port_Struct_C* p, uint16_t offset, uint16_t value);

    void (*writeDoubleWord_IO_C)(struct IO_Port_Struct_C* p, uint32_t value);

    void (*writeDoubleWord_IO_off_C)(struct IO_Port_Struct_C* p, uint16_t offset, uint32_t value);

    uint8_t (*readByte_IO_C)(struct IO_Port_Struct_C* p);

    uint8_t (*readByte_IO_off_C)(struct IO_Port_Struct_C* p, uint16_t offset);

    uint16_t (*readWord_IO_C)(struct IO_Port_Struct_C* p);

    uint16_t (*readWord_IO_off_C)(struct IO_Port_Struct_C* p, uint16_t offset);

    uint32_t (*readDoubleWord_IO_C)(struct IO_Port_Struct_C* p);

    uint32_t (*readDoubleWord_IO_off_C)(struct IO_Port_Struct_C* p, uint16_t offset);

    IO_Port_C io_port_pointer;
};

typedef struct IO_Port_Struct_C IO_Port_Struct_C;

#ifdef __cplusplus
extern "C" {
#endif

void newIO_Port(IO_Port_Struct_C* port_struct, uint16_t address);

void writeByte_IO_C(IO_Port_Struct_C* p, uint8_t value);

void writeByte_IO_off_C(IO_Port_Struct_C* p, uint16_t offset, uint8_t value);

void writeWord_IO_C(IO_Port_Struct_C* p, uint16_t value);

void writeWord_IO_off_C(IO_Port_Struct_C* p, uint16_t offset, uint16_t value);

void writeDoubleWord_IO_C(IO_Port_Struct_C* p, uint32_t value);

void writeDoubleWord_IO_off_C(IO_Port_Struct_C* p, uint16_t offset, uint32_t value);

uint8_t readByte_IO_C(IO_Port_Struct_C* p);

uint8_t readByte_IO_off_C(IO_Port_Struct_C* p, uint16_t offset);

uint16_t readWord_IO_C(IO_Port_Struct_C* p);

uint16_t readWord_IO_off_C(IO_Port_Struct_C* p, uint16_t offset);

uint32_t readDoubleWord_IO_C(IO_Port_Struct_C* p);

uint32_t readDoubleWord_IO_off_C(IO_Port_Struct_C* p, uint16_t offset);

#ifdef __cplusplus
}
#endif

#endif
