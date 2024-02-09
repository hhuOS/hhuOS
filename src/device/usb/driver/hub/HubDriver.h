#ifndef HUB_DRIVER__INCLUDE
#define HUB_DRIVER__INCLUDE

#include "../UsbDriver.h"
#include "stdint.h"
#include "../../interfaces/LoggerInterface.h"
#include "../../include/UsbInterface.h"

// this driver is the only one which is invoked at enumeration time !

#define HUB_DESCRIPTOR_VARIABLE_X 64

//#define HUB_DRIVER_DUMP_PORTS don't need to activate when using emulated hubs .

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

struct HubDescriptor{
    uint8_t len;
    uint8_t type;
    uint8_t num_ports;
    uint16_t hub_chars;
    uint8_t potpgt;
    uint8_t max_hub_current;
    uint8_t x[HUB_DESCRIPTOR_VARIABLE_X];
} __attribute__((packed));

struct HubDriver{
    struct UsbDriver super;
    struct HubDev dev;
    void (*new_hub_driver)(struct HubDriver* driver, char* name, struct UsbDevice_ID* entry);

    int (*read_hub_status)(struct HubDriver* driver, UsbDev* dev, Interface* itf, uint8_t* data, unsigned int len);
    int (*read_hub_descriptor)(struct HubDriver* driver, UsbDev* dev, Interface* itf, uint8_t* data);
    int (*configure_hub)(struct HubDriver* driver);
    int (*check_valid_hub_transfer)(struct HubDriver* driver);
    int (*set_hub_feature)(struct HubDriver* driver, UsbDev* dev, Interface* itf, uint16_t port, uint16_t feature);
    int (*clear_hub_feature)(struct HubDriver* driver, UsbDev* dev, Interface* itf, uint16_t port, uint16_t feature);

    void (*dump_port_status_change)(struct HubDriver* driver, uint16_t* port_status_change_field);
    void (*dump_port_status)(struct HubDriver* driver, uint16_t* port_status_field);

    Logger_C* (*init_hub_driver_logger)(struct HubDriver* driver);

    struct HubDescriptor hub_desc;
    uint8_t x_powered; 
    uint8_t x_wakeup;

    uint8_t transfer_success;

    Logger_C* hub_driver_logger;
};

enum FeatureSelectorHub{
    PORT_CONNECTION = 0x00,
    PORT_ENABLE = 0x01,
    PORT_SUSPEND = 0x02,
    PORT_OVER_CURRENT = 0x03,
    PORT_RESET = 0x04,
    PORT_LINK_STATE = 0x05,
    PORT_POWER = 0x08,
    PORT_LOW_SPEED = 0x09,
    C_PORT_CONNECTION = 0x10,
    C_PORT_ENABLE = 0x11,
    C_PORT_SUSPEND = 0x12,
    C_PORT_OVER_CURRENT = 0x13,
    C_PORT_RESET = 0x14,
    PORT_TEST = 0x15,
    PORT_INDICATOR = 0x16,
    PORT_U1_TIMEOUT = 0x17,
    PORT_U2_TIMEOUT = 0x18,
    C_PORT_LINK_STATE = 0x19,
    C_PORT_CONFIG_ERROR = 0x1A,
    PORT_REMOTE_WAKE_MASK = 0x1B,
    BH_PORT_RESET = 0x1C,
    C_BH_PORT_RESET = 0x1D,
    FORCE_LINKPM_ACCEPT = 0x1E
};

enum Port_Status_Field{
    PORT_INDICATOR_STATUS = 0x0C,
    PORT_TEST_STATUS = 0x0B,
    PORT_DEVICE_ATTACHED_HIGH_SPEED_STATUS = 0x0A,
    PORT_DEVICE_ATTACHED_LOW_SPEED_STATUS = 0x09,
    PORT_POWER_STATUS = 0x08,
    PORT_RESET_STATUS = 0x04,
    PORT_OVER_CURRENT_STATUS = 0x03,
    PORT_SUSPEND_STATUS = 0x02,
    PORT_ENABLE_STATUS = 0x01,
};

enum Port_Status_Change_Field{
    PORT_RESET_CHANGE_STATUS = 0x04,
    PORT_OVER_CURRENT_CHANGE_STATUS = 0x03,
    PORT_SUSPEND_CHANGE_STATUS = 0x02,
    PORT_ENABLE_CHANGE_STATUS = 0x01,
};

typedef struct HubDev HubDev;
typedef struct HubDriver HubDriver;
typedef struct HubDescriptor HubDescriptor;
typedef enum FeatureSelectorHub FeatureSelectorHub;

void callback_hub(UsbDev* dev, uint32_t status, void* data);

int16_t probe_hub(UsbDev* dev, Interface* interface);
void disconnect_hub(UsbDev* dev, Interface* interface);

void new_hub_driver(HubDriver* driver, char* name, UsbDevice_ID* entry);

int read_hub_status(HubDriver* driver, UsbDev* dev, Interface* itf, uint8_t* data, unsigned int len);
int read_hub_descriptor(HubDriver* driver, UsbDev* dev, Interface* itf, uint8_t* data);
int configure_hub(HubDriver* driver);
void configure_callback(UsbDev* dev, uint32_t status, void* data);
int set_hub_feature(HubDriver* driver, UsbDev* dev, Interface* itf, uint16_t port, uint16_t feature);
int clear_hub_feature(HubDriver* driver, UsbDev* dev, Interface* itf, uint16_t port, uint16_t feature);

void dump_port_status_change(HubDriver* driver, uint16_t* port_status_change_field);
void dump_port_status(HubDriver* driver, uint16_t* port_status_field);

Logger_C* init_hub_driver_logger(HubDriver* driver);

#endif