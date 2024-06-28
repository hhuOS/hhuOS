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

#define __INIT_HUB_DRIVER__(name, driver_name, entry) \
    __ENTRY__(name, read_hub_status) = &read_hub_status; \
    __ENTRY__(name, read_hub_descriptor) = &read_hub_descriptor; \
    __ENTRY__(name, configure_hub) = &configure_hub; \
    __ENTRY__(name, set_hub_feature) = &set_hub_feature; \
    __ENTRY__(name, clear_hub_feature) = &clear_hub_feature; \
    __ENTRY__(name, dump_port_status) = &dump_port_status; \
    __ENTRY__(name, dump_port_status_change) = &dump_port_status_change; \
    __ENTRY__(name, is_device_removable) = &is_device_removable; \
    __ENTRY__(name, get_free_hub_dev) = &get_free_hub_dev; \
    __ENTRY__(name, match_hub_dev) = &match_hub_dev; \
    __ENTRY__(name, free_hub_dev) = &free_hub_dev; \
    __ENTRY__(name, get_hub_descriptor) = &get_hub_descriptor; \
    \
    __SUPER__(name, probe) = &probe_hub; \
    __SUPER__(name, disconnect) = &disconnect_hub; \
    __SUPER__(name, new_usb_driver) = &new_usb_driver; \
    \
    __CALL_SUPER__(name->super, new_usb_driver, driver_name, entry)

struct HubDescriptor{
    uint8_t len;
    uint8_t type;
    uint8_t num_ports;
    uint16_t hub_chars;
    uint8_t potpgt;
    uint8_t max_hub_current;
    uint8_t x[HUB_DESCRIPTOR_VARIABLE_X];
} __attribute__((packed));

struct HubDev{
    UsbDev* usb_dev;
    unsigned int endpoint_addr;
    void* buffer;
    unsigned int buffer_size;
    uint8_t priority;
    Interface* interface;
    uint16_t interval;
    UsbDriver* driver;

    struct HubDescriptor hub_desc;
    uint8_t x_powered; 
    uint8_t x_wakeup;

    uint8_t transfer_success;

    void (*callback)(UsbDev* dev, Interface* interface, uint32_t status, void* data);
};

struct HubDriver{
    struct UsbDriver super;
    struct HubDev dev[MAX_DEVICES_PER_USB_DRIVER];
    uint8_t hub_map[MAX_DEVICES_PER_USB_DRIVER];

    void (*new_hub_driver)(struct HubDriver* driver, char* name, struct UsbDevice_ID* entry);

    int (*read_hub_status)(struct HubDriver* driver, struct HubDev* dev, Interface* itf, uint8_t* data, unsigned int len);
    int (*read_hub_descriptor)(struct HubDriver* driver, struct HubDev* dev, Interface* itf, uint8_t* data);
    int (*configure_hub)(struct HubDriver* driver);
    int (*check_valid_hub_transfer)(struct HubDriver* driver);
    int (*set_hub_feature)(struct HubDriver* driver, struct HubDev* dev, Interface* itf, uint16_t port, uint16_t feature);
    int (*clear_hub_feature)(struct HubDriver* driver, struct HubDev* dev, Interface* itf, uint16_t port, uint16_t feature);

    void (*dump_port_status_change)(struct HubDriver* driver, uint16_t* port_status_change_field);
    void (*dump_port_status)(struct HubDriver* driver, uint16_t* port_status_field);
    uint8_t (*is_device_removable)(struct HubDriver* driver, struct HubDev* dev, uint8_t downstream_port);
    void (*get_hub_descriptor)(struct HubDriver* driver, UsbDev* dev, UsbDeviceRequest* device_req,
        void* data, unsigned int len, callback_function callback);
    struct HubDev* (*get_free_hub_dev)(struct HubDriver* driver);
    struct HubDev* (*match_hub_dev)(struct HubDriver* driver, UsbDev* dev);
    void (*free_hub_dev)(struct HubDriver* driver, struct HubDev* hub_dev);
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

void new_hub_driver(HubDriver* driver, char* name, UsbDevice_ID* entry);

#endif