#include "PciDeviceInterface.h"
#include "../../../device/pci/PciDevice.h"

uint8_t readByte_c(struct PciDevice_Struct* pci_struct, uint8_t reg){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->readByte(reg);
}
uint16_t readWord_c(struct PciDevice_Struct* pci_struct, uint8_t reg){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->readWord(reg);
}
uint32_t readDoubleWord_c(struct PciDevice_Struct* pci_struct, uint8_t reg){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->readDoubleWord(reg);
}

void writeByte_c(struct PciDevice_Struct* pci_struct, uint8_t reg, uint8_t value){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->writeByte(reg,value);
}

void writeWord_c(struct PciDevice_Struct* pci_struct, uint8_t reg, uint16_t value){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->writeWord(reg,value);
}

void writeDoubleWord_c(struct PciDevice_Struct* pci_struct, uint8_t reg, uint32_t value){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->writeDoubleWord(reg,value);
}

uint8_t get_bus_c(struct PciDevice_Struct* pci_struct){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->getBus();
}

uint8_t get_device_c(struct PciDevice_Struct* pci_struct){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->getDevice();
}

uint8_t get_function_c(PciDevice_Struct* pci_struct){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->getFunction();
}

uint16_t get_vendor_id_c(PciDevice_Struct* pci_struct){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->getVendorId();
}

uint16_t get_device_id_c(PciDevice_Struct* pci_struct){
    return ((Device::PciDevice*)pci_struct->pci_dev_pointer)->getDeviceId();
}

uint8_t get_interrupt_line_c(PciDevice_Struct* pci_struct){
    return (uint8_t)(((Device::PciDevice*)pci_struct->pci_dev_pointer)->getInterruptLine());
}

void new_pci_device_c(struct PciDevice_Struct* pci_struct, PciDevice_C pci_dev_pointer){
    pci_struct->pci_dev_pointer = pci_dev_pointer;
    pci_struct->readByte_c = &readByte_c;
    pci_struct->readWord_c = &readWord_c;
    pci_struct->readDoubleWord_c = &readDoubleWord_c;
    pci_struct->writeByte_c = &writeByte_c;
    pci_struct->writeWord_c = &writeWord_c;
    pci_struct->writeDoubleWord_c = &writeDoubleWord_c;
    pci_struct->get_interrupt_line_c = &get_interrupt_line_c;
    pci_struct->get_device_id_c = &get_device_id_c;
    pci_struct->get_vendor_id_c = &get_vendor_id_c;
    pci_struct->get_function_c = &get_function_c;
    pci_struct->get_device_c = &get_device_c;
    pci_struct->get_bus_c = &get_bus_c;
}