#ifndef PciDevice_C__include
#define PciDevice_C__include

typedef void* PciDevice_C;

#include "stdint.h"

struct PciDevice_Struct{
    uint8_t (*readByte_c)(struct PciDevice_Struct* pci_struct, uint8_t reg);
    uint16_t (*readWord_c)(struct PciDevice_Struct* pci_struct, uint8_t reg);
    uint32_t (*readDoubleWord_c)(struct PciDevice_Struct* pci_struct, uint8_t reg);

    void (*writeByte_c)(struct PciDevice_Struct* pci_struct, uint8_t reg, uint8_t value);
    void (*writeWord_c)(struct PciDevice_Struct* pci_struct, uint8_t reg, uint16_t value);
    void (*writeDoubleWord_c)(struct PciDevice_Struct* pci_struct, uint8_t reg, uint32_t value);

    void (*new_pci_device_c)(struct PciDevice_Struct* pci_struct, PciDevice_C pci_dev_pointer);

    uint8_t (*get_interrupt_line_c)(struct PciDevice_Struct* pci_struct);
    uint16_t (*get_device_id_c)(struct PciDevice_Struct* pci_struct);
    uint16_t (*get_vendor_id_c)(struct PciDevice_Struct* pci_struct);
    uint8_t (*get_function_c)(struct PciDevice_Struct* pci_struct);
    uint8_t (*get_device_c)(struct PciDevice_Struct* pci_struct);
    uint8_t (*get_bus_c)(struct PciDevice_Struct* pci_struct);


    PciDevice_C pci_dev_pointer;
};

typedef struct PciDevice_Struct PciDevice_Struct;


#ifdef __cplusplus
#include <cstdint>

extern "C" {
#endif

void new_pci_device_c(struct PciDevice_Struct* pci_struct, PciDevice_C pci_dev_pointer);

uint8_t get_interrupt_line_c(PciDevice_Struct* pci_struct);
uint16_t get_device_id_c(PciDevice_Struct* pci_struct);
uint16_t get_vendor_id_c(PciDevice_Struct* pci_struct);
uint8_t get_function_c(PciDevice_Struct* pci_struct);
uint8_t get_device_c(struct PciDevice_Struct* pci_struct);
uint8_t get_bus_c(struct PciDevice_Struct* pci_struct);

uint8_t readByte_c(struct PciDevice_Struct* pci_struct, uint8_t reg);
uint16_t readWord_c(struct PciDevice_Struct* pci_struct, uint8_t reg);
uint32_t readDoubleWord_c(struct PciDevice_Struct* pci_struct, uint8_t reg);

void writeByte_c(struct PciDevice_Struct* pci_struct, uint8_t reg, uint8_t value);
void writeWord_c(struct PciDevice_Struct* pci_struct, uint8_t reg, uint16_t value);
void writeDoubleWord_c(struct PciDevice_Struct* pci_struct, uint8_t reg, uint32_t value);

#ifdef __cplusplus    
}    
#endif

#endif