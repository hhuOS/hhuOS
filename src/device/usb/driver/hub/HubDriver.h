#ifndef HUB_DRIVER__INCLUDE
#define HUB_DRIVER__INCLUDE

#include "../UsbDriver.h"
#include "stdint.h"

#define HUB_DESCRIPTOR_VARIABLE_X 64

#define SELF_POWERED 0x01
#define REMOTE_WAKEUP 0x02

struct HubDev{
    UsbDev* usb_dev;
    unsigned int endpoint_addr;
    void* buffer;
    unsigned int buffer_size;
    uint8_t priority;
    Interface* interface;
    uint16_t interval;
    UsbDriver* driver;

    void (*callback)(UsbDev* dev, uint32_t status, void* data);
};

struct HubDriver{
    struct UsbDriver super;
    struct HubDev dev;
    void (*new_hub_driver)(struct HubDriver* driver, char* name, struct UsbDevice_ID* entry);

    HubDescriptor hub_desc;
    uint8_t x_powered; 
    uint8_t x_wakeup;

    uint8_t success_transfer;
    uint8_t callback_invoked;
};

struct HubDescriptor{
    uint8_t len;
    uint8_t type;
    uint8_t num_ports;
    uint16_t hub_chars;
    uint8_t potpgt;
    uint8_t max_hub_current;
    uint8_t x[HUB_DESCRIPTOR_VARIABLE_X];
};

typedef struct HubDev HubDev;
typedef struct HubDriver HubDriver;
typedef struct HubDescriptor HubDescriptor;

void callback_hub(UsbDev* dev, uint32_t status, void* data);

int16_t probe_hub(UsbDev* dev, Interface* interface);
void disconnect_hub(UsbDev* dev, Interface* interface);

#endif