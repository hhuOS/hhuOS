#include "PciDeviceInterface.h"

#ifndef PCI_C__include
#define PCI_C__include

struct Pci_C{
    PciDevice_Struct* pci_devices;
    uint8_t devices_length;
};

typedef struct Pci_C Pci_C;

enum Register_C {
    VENDOR_ID = 0x00,
    DEVICE_ID = 0x02,
    COMMAND = 0x04,
    STATUS = 0x06,
    REVISION = 0x08,
    PROGRAMMING_INTERFACE = 0x09,
    SUBCLASS = 0x0a,
    CLASS = 0x0b,
    CACHE_LINE_SIZE = 0x0c,
    MASTER_LATENCY_TIMER = 0x0d,
    HEADER_TYPE = 0x0e,
    BIST = 0x0f,
    BASE_ADDRESS_0 = 0x10,
    BASE_ADDRESS_1 = 0x14,
    BASE_ADDRESS_2 = 0x18,
    BASE_ADDRESS_3 = 0x1C,
    BASE_ADDRESS_4 = 0x20,
    BASE_ADDRESS_5 = 0x24,
    CARDBUS_CIS_POINTER = 0x28,
    SUBSYSTEM_VENDOR_ID = 0x2c,
    SUBSYSTEM_ID = 0x2e,
    EXPANSION_ROM_BASE_ADDRESS = 0x30,
    CAPABILITIES_POINTER = 0x34,
    INTERRUPT_LINE = 0x3c,
    INTERRUPT_PIN = 0x3d,
    MIN_GRANT = 0x3e,
    MAX_LATENCY = 0x3f,
    SECONDARY_BUS = 0x19,
};

enum Command_C {
    IO_SPACE = 0x0001,
    MEMORY_SPACE = 0x0002,
    BUS_MASTER = 0x0004,
    SPECIAL_CYCLES = 0x0008,
    MEMORY_WRITE_AND_INVALIDATE = 0x0010,
    VGA_PALETTE_SNOOP = 0x0020,
    PARITY_ERROR_RESPONSE = 0x0040,
    SERR_ENABLE = 0x0100,
    FAST_BACK_TO_BACK = 0x0200,
    INTERRUPT_DISABLE = 0x0400
};

enum Status_C {
    INTERRUPT = 0x0008,
    CAPABILITIES_LIST = 0x0010,
    CAPABLE_OF_66_MHZ = 0x0020,
    FAST_BACK_TO_BACK_CAPABLE = 0x0080,
    MASTER_DATA_PARITY_ERROR = 0x0100,
    DEVSEL_TIMING = 0x0200,
    SIGNALED_TARGET_ABORT = 0x0800,
    RECEIVED_TARGET_ABORT = 0x1000,
    RECEIVED_MASTER_ABORT = 0x2000,
    SIGNALED_SYSTEM_ERROR = 0x4000,
    DETECTED_PARITY_ERROR = 0x8000
};

enum Class_C {
    UNCLASSIFIED = 0x00,
    MASS_STORAGE = 0x01,
    NETWORK_CONTROLLER = 0x02,
    DISPLAY_CONTROLLER = 0x03,
    MULTIMEDIA_CONTROLLER = 0x04,
    MEMORY_CONTROLLER = 0x05,
    BRIDGE = 0x06,
    SIMPLE_COMMUNICATION_CONTROLLER = 0x07,
    BASE_SYSTEM_PERIPHERAL = 0x08,
    INPUT_DEVICE_CONTROLLER = 0x09,
    DOCKING_STATION = 0x0a,
    PROCESSOR = 0x0b,
    SERIAL_BUS_CONTROLLER = 0x0c,
    WIRELESS_CONTROLLER = 0x0d,
    INTELLIGENT_CONTROLLER = 0x0e,
    SATELLITE_COMMUNICATION_CONTROLLER = 0x0f,
    ENCRYPTION_CONTROLLER = 0x10,
    SIGNAL_PROCESSING_CONTROLLER = 0x11,
    PROCESSING_ACCELERATOR = 0x12,
    NON_ESSENTIAL_INSTRUMENTATION = 0x13,
    CO_PROCESSOR = 0x40,
    UNASSIGNED = 0xff
};

#ifdef __cplusplus
#include <cstdint>

extern "C" {
#endif

Pci_C* search_c_one_p(uint16_t vendorID);
Pci_C* search_c_two_p(uint16_t vendorID, uint16_t deviceID);
Pci_C* search_c_controllers(uint8_t classID, uint8_t subclassID, uint8_t interface);

#ifdef __cplusplus
}

#endif

#endif
