#include "IoPortInterface.h"
#include "../../cpu/IoPort.h"

void newIO_Port(IO_Port_Struct_C* port_struct, uint16_t address){
    port_struct->io_port_pointer = (IO_Port_C)new Device::IoPort(address);
    port_struct->writeByte_IO_C = &writeByte_IO_C;
    port_struct->writeByte_IO_off_C = &writeByte_IO_off_C;
    port_struct->writeWord_IO_C = &writeWord_IO_C;
    port_struct->writeWord_IO_off_C = &writeWord_IO_off_C;
    port_struct->writeDoubleWord_IO_C = &writeDoubleWord_IO_C;
    port_struct->writeDoubleWord_IO_off_C = &writeDoubleWord_IO_off_C;

    port_struct->readByte_IO_C = &readByte_IO_C;
    port_struct->readByte_IO_off_C = &readByte_IO_off_C;
    port_struct->readWord_IO_C = &readWord_IO_C;
    port_struct->readWord_IO_off_C = &readWord_IO_off_C;
    port_struct->readDoubleWord_IO_C = &readDoubleWord_IO_C;
    port_struct->readDoubleWord_IO_off_C = &readDoubleWord_IO_off_C;
}

void writeByte_IO_C(IO_Port_Struct_C* p, uint8_t value){
    ((Device::IoPort*)p->io_port_pointer)->writeByte(value);
}

void writeByte_IO_off_C(IO_Port_Struct_C* p, uint16_t offset, uint8_t value){
    ((Device::IoPort*)p->io_port_pointer)->writeByte(offset,value);
}

void writeWord_IO_C(IO_Port_Struct_C* p, uint16_t value){
    ((Device::IoPort*)p->io_port_pointer)->writeWord(value);
}

void writeWord_IO_off_C(IO_Port_Struct_C* p, uint16_t offset, uint16_t value){
    ((Device::IoPort*)p->io_port_pointer)->writeWord(offset,value);
}

void writeDoubleWord_IO_C(IO_Port_Struct_C* p, uint32_t value){
    ((Device::IoPort*)p->io_port_pointer)->writeDoubleWord(value);
}

void writeDoubleWord_IO_off_C(IO_Port_Struct_C* p, uint16_t offset, uint32_t value){
    ((Device::IoPort*)p->io_port_pointer)->writeDoubleWord(offset,value);
}

uint8_t readByte_IO_C(IO_Port_Struct_C* p){
    return ((Device::IoPort*)p->io_port_pointer)->readByte();
}

uint8_t readByte_IO_off_C(IO_Port_Struct_C* p, uint16_t offset){
    return ((Device::IoPort*)p->io_port_pointer)->readByte(offset);
}

uint16_t readWord_IO_C(IO_Port_Struct_C* p){
    return ((Device::IoPort*)p->io_port_pointer)->readWord();
}

uint16_t readWord_IO_off_C(IO_Port_Struct_C* p, uint16_t offset){
    return ((Device::IoPort*)p->io_port_pointer)->readWord(offset);
}

uint32_t readDoubleWord_IO_C(IO_Port_Struct_C* p){
    return ((Device::IoPort*)p->io_port_pointer)->readDoubleWord();
}

uint32_t readDoubleWord_IO_off_C(IO_Port_Struct_C* p, uint16_t offset){
    return ((Device::IoPort*)p->io_port_pointer)->readDoubleWord(offset);
}