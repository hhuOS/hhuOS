#ifndef UsbDriver__include
#define UsbDriver__include

#include "stdint.h"
#include "../events/listeners/EventListener.h"
#include "../events/EventDispatcher.h"
#include "../utility/Utils.h"
#include "../dev/UsbDevice.h"
#include "../include/UsbInterface.h"
#include "../interfaces/LoggerInterface.h"

#define MAX_DEVICES_PER_USB_DRIVER 10

#define __DEVICE_ITERATE__(dtype, name) \
    __FOR_RANGE__(name, dtype, 0, MAX_DEVICES_PER_USB_DRIVER)

struct UsbDevice_ID{
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice_low; // minor+subminor part 0x00MN
    uint16_t bcdDevice_high;// major part 0xJJ00 // format of bcd 0xJJMN , JJ major , M minor and N sub minor
    // low -> minor + sub minor , high major

    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;

    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
};

typedef struct UsbDevice_ID UsbDevice_ID;

#define USB_DEVICE(vendor, product) \
(struct UsbDevice_ID){.idVendor = vendor, .idProduct = product, \
.bcdDevice_low = 0xFF, .bcdDevice_high = 0xFF, .bDeviceClass = 0xFF, .bDeviceSubClass = 0xFF, .bDeviceProtocol = 0xFF, \
.bInterfaceClass = 0xFF, .bInterfaceSubClass = 0xFF, .bInterfaceProtocol = 0xFF}

#define USB_DEVICE_VER(vendor, product, low, high) \
(struct UsbDevice_ID){.idVendor = vendor, .idProduct = product, \
.bcdDevice_low = low, .bcdDevice_high = high, .bDeviceClass = 0xFF, .bDeviceSubClass = 0xFF, .bDeviceProtocol = 0xFF, \
.bInterfaceClass = 0xFF, .bInterfaceSubClass = 0xFF, .bInterfaceProtocol = 0xFF}

#define USB_DEVICE_INFO(class_d, subclass, protocol) \
(struct UsbDevice_ID){.idVendor = 0xFF, .idProduct = 0xFF, \
.bcdDevice_low = 0xFF, .bcdDevice_high = 0xFF, .bDeviceClass = class_d, .bDeviceSubClass = subclass, .bDeviceProtocol = protocol, \
.bInterfaceClass = 0xFF, .bInterfaceSubClass = 0xFF, .bInterfaceProtocol = 0xFF}

#define USB_INTERFACE_INFO(class_i, subclass, protocol) \
(struct UsbDevice_ID){.idVendor = 0xFF, .idProduct = 0xFF, \
.bcdDevice_low = 0xFF, .bcdDevice_high = 0xFF, .bDeviceClass = 0xFF, .bDeviceSubClass = 0xFF, .bDeviceProtocol = 0xFF, \
.bInterfaceClass = class_i, .bInterfaceSubClass = subclass, .bInterfaceProtocol = protocol}

#define USB_MAJOR_VERSION(bcd) ((bcd & 0xFF00) >> 8)
#define USB_MINOR_VERSION(bcd) ((bcd & 0x00FF))

#define USB_MINOR_PART_VERSION(bcd) ((bcd & 0x00F0) >> 4)
#define USB_SUB_MINOR_PART_VERSION(bcd) ((bcd & 0x000F))

#define __INIT_USB_DRIVER__(name) \
    __ENTRY__(name, get_device_id) = &get_device_id_table; \
    __ENTRY__(name, get_device_head) = &get_head_device; \
    __ENTRY__(name, set_event_dispatcher) = &set_event_dispatcher

#define __GET_FREE_DEV__(type, internal_dev, bit_map) \
    type* __dev = 0; \
    __FOR_RANGE__(i, int, 0, MAX_DEVICES_PER_USB_DRIVER){ \
        __IF_CUSTOM__(__IS_ZERO__(bit_map[i]), bit_map[i] = 1; \
            __dev = internal_dev + i; return __dev); \
    }   \
    return __dev

#define __FREE_DEV__(container_device, internal_dev, bit_map) \
    __FOR_RANGE__(i, int, 0, MAX_DEVICES_PER_USB_DRIVER) { \
        __IF_CUSTOM__((internal_dev + i) == container_device, \
            bit_map[i] = 0; return); \
    }

#define __MATCH_DEV__(type, internal_dev, dev_field, device) \
    type* __dev = 0; \
    __FOR_RANGE__(i, int, 0, MAX_DEVICES_PER_USB_DRIVER){ \
        __IF_CUSTOM__((internal_dev + i)->dev_field == device, \
        __dev = internal_dev + i; return __dev); \
    } \
    return __dev

#define __REQUEST_OR_LEAVE__(type, driver, request_function, name) \
    type* name = __STRUCT_CALL__(driver, request_function); \
    __IF_RET_NEG__(__IS_NULL__(name))

#define __DRIVER_FAIL_ROUTINE__(fail_routine) \
    fail_routine; continue

#define __DRIVER_DIRECTION_IN__(fail_routine, dev, endpoint) \
    __IF_CUSTOM__(__STRUCT_CALL__(dev, __is_direction_out, endpoint), \
        __DRIVER_FAIL_ROUTINE__(fail_routine))

#define __DRIVER_DIRECTION_OUT__(fail_routine, dev, endpoint) \
    __IF_CUSTOM__(__STRUCT_CALL__(dev, __is_direction_in, endpoint), \
        __DRIVER_FAIL_ROUTINE__(fail_routine))

#define __DRIVER_TYPE_INTERRUPT__(fail_routine, dev, endpoint) \
    __IF_CUSTOM__(!(__STRUCT_CALL__(dev, __is_interrupt_type, endpoint)), \
        __DRIVER_FAIL_ROUTINE__(fail_routine))

#define __DRIVER_TYPE_CONTROL__(fail_routine, dev, endpoint) \
    __IF_CUSTOM__(!(__STRUCT_CALL__(dev, __is_control_type, endpoint)), \
        __DRIVER_FAIL_ROUTINE__(fail_routine))

#define __DRIVER_TYPE_BULK__(fail_routine, dev, endpoint) \
    __IF_CUSTOM__(!(__STRUCT_CALL__(dev, __is_bulk_type, endpoint)), \
        __DRIVER_FAIL_ROUTINE__(fail_routine))

#define __DRIVER_LOGGER_INFO_CALL__(sub_driver, message, ...) \
    __STRUCT_CALL__((__CAST__(UsbDriver*, sub_driver))->driver_logger, info_c, \
        message, ## __VA_ARGS__)

#define __DRIVER_LOGGER_DEBUG_CALL__(sub_driver, message, ...) \
    __STRUCT_CALL__((__CAST__(UsbDriver*, sub_driver))->driver_logger, debug_c, \
        message, ## __VA_ARGS__)

struct UsbDriver{

    void (*new_usb_driver)(struct UsbDriver* usb_driver, char* name, struct UsbDevice_ID* entry);
    int16_t (*probe)(UsbDev* dev, Interface* interface);
    void (*disconnect)(UsbDev* dev, Interface* interface);

    char* name;
    UsbDevice_ID* entry;

    __DECLARE_STRUCT_GET__(device_id, struct UsbDriver*, UsbDevice_ID*);
    __DECLARE_STRUCT_GET__(device_head, struct UsbDriver*, list_head);
    __DECLARE_STRUCT_SET__(event_dispatcher, struct UsbDriver*, EventDispatcher*);

    list_head head;   // linking devices
    list_element l_e; // element in controller linkage

    int listener_id;

    EventDispatcher* dispatcher; // set by appropriate UsbController
    Logger_C* driver_logger;
    SuperMap* interface_buffer_map;
    SuperMap* interface_write_callback_map;
};

typedef struct UsbDriver UsbDriver;

__DECLARE_GET__(UsbDevice_ID*, device_id_table, entry, UsbDriver*, static inline);
__DECLARE_GET__(list_head, head_device, head, UsbDriver*, static inline);
__DECLARE_SET__(event_dispatcher, dispatcher, UsbDriver*, EventDispatcher*, static inline);

static inline void new_usb_driver(UsbDriver* usb_driver, char* name, UsbDevice_ID* entry) {
    usb_driver->name = name;
    usb_driver->entry = entry;
    usb_driver->head.l_e = 0;
    usb_driver->l_e.l_e = 0;

    Logger_C* driver_logger = (Logger_C*)interface_allocateMemory(
        sizeof(Logger_C), 0);
    __STRUCT_INIT__(driver_logger, new_logger, new_logger, 
        USB_DRIVER_LOGGER_TYPE, LOGGER_LEVEL_INFO);
    Interface_Buffer_Map* buffer_map = (Interface_Buffer_Map*)interface_allocateMemory(
        sizeof(Interface_Buffer_Map), 0);
    Interface_Write_Callback_Map* write_callback_map = (Interface_Write_Callback_Map*)interface_allocateMemory(
        sizeof(Interface_Write_Callback_Map), 0);
    __STRUCT_INIT__(buffer_map, new_map, newInterface_Buffer_Map, 
        "Map<Interface*,void*>");
    __STRUCT_INIT__(write_callback_map, new_map, newInterface_Write_Callback_Map,
        "Map<Interface*, write_callback*");
    usb_driver->driver_logger = driver_logger;
    usb_driver->interface_buffer_map = (SuperMap*)buffer_map;
    usb_driver->interface_write_callback_map = (SuperMap*)write_callback_map;
    __INIT_USB_DRIVER__(usb_driver);
}

typedef void (*write_callback)(uint8_t* map_io_buffer, uint16_t len, void* buffer);

// used for stream writes
static inline void driver_stream_write_register_buffer(UsbDriver* driver, Interface* interface, 
    void* buffer, write_callback w){
    __MAP_PUT__(driver->interface_buffer_map, interface, buffer);
    __STRUCT_CALL__(driver->interface_write_callback_map, put_c, interface, (void*
    )w);
}

/******************************
 * Device Classes
*******************************/
#define AUDIO 0x01
#define COMMUNICATION_X_CDC_CONTROL 0x02
#define HID_CLASS 0x03
#define DEVICE_CLASS_PID 0x05
#define IMAGE 0x06
#define PRINTER 0x07
#define DEVICE_CLASS_MASS_STORAGE 0x08
#define HUB 0x09
#define CDC_DATA 0x0A
#define SMART_CARD 0x0B
#define CONTENT_SECURITY 0x0D
#define VIDEO 0x0E
#define PERSON_HEALTHCARE 0x0F
#define AV 0x10
#define BILLBOARD 0x11
#define TYPE_C_BRIDGE 0x12
#define DIAGNOSTIC_DEV 0xDC
#define DEVICE_CLASS_WIRELESS_CONTROLLER 0xE0

/*****************************
 * Device Sub Classes
******************************/

// Audio Class
#define AUDIO_CONTROL 0x01
#define AUDIO_STREAMING 0x02
#define MIDI_STREAMING 0x03

// CDC Control Class
#define DIRECT_LINE_CONTROL_MODEL 0x00
#define ABSTRACT_CONTROL_MODEL 0x01
#define TELEPHONE_CONTROL_MODEL 0x02
#define MULTI_CHANNEL_CONTROL_MODEL 0x03
#define CAPI_CONTROL_MODEL 0x04
#define ETHERNET_NETWORKING_CONTROL_MODEL 0x05
#define ATM_NETWORKING_CONTROL_MODEL 0x06
#define WIRELESS_HANDSET_CONTROL_MODEL 0x07
#define DEVICE_MANAGEMENT 0x08

// HID
#define BOOT_INTERFACE 0x01
#define HID_MOUSE 0x02
#define HID_KBD 0x03
#define KEY_PAD 0x04
#define MULTI_AXIS_CONTROLLER 0x05
#define TABLET 0x06

// Mass Storage
#define SCSI_COMMAND_SET 0x00
#define RBC 0x01
#define UFI 0x02
#define SFF 0x03
#define IEEE 0x04
#define UOTP 0x05
#define SCSI 0x06

//Hub
#define HUB_FULL_SPEED 0x00
#define STT 0x01
#define TT 0x02
#define MULTI_TT 0x03

//CDC
#define SERIAL_MOUSE 0x01

// Smart Card
#define SMART_CARD_READER_INTERFACE 0x01

// Video
#define VIDEO_CONTROL 0x01
#define VIDEO_STREAMING 0x02

// Wireless
#define BLUETOOTH 0x01
#define RF_CONTROLLER 0x02
#define WI_FI 0x03
#define UWB_RADIO_CONTROL 0x04

// Miscellaneous
#define COMMON_CLASS 0x01
#define INTERFACE_ASSOCIATION 0x02
#define WIRE_ADAPTER_MULTI_USE_TERMINAL 0x03

/*****************************
 * Device Protocols
******************************/

/*****************************
 * Interface Classes
******************************/
#define AUDIO_INTERFACE 0x01
#define CDC_INTERFACE 0x02
#define HID_INTERFACE 0x03
#define PID_INTERFACE 0x05
#define IMAGE_INTERFACE 0x06
#define PRINTER_INTERFACE 0x07
#define MASS_STORAGE_INTERFACE 0x08
#define HUB_INTERFACE 0x09
#define CDC_DATA_INTERFACE 0x0A
#define SMART_CARD_INTERFACE 0x0B
#define CONTENT_SECURITY_INTERFACE 0x0D
#define VIDEO_INTERFACE 0x0E
#define WIRELESS_CONTROLLER_INTERFACE 0xE0
#define MISCELLANEOUS_INTERFACE 0xEF

/*****************************
 * Interface Sub Classes
******************************/
// AUDIO
#define AUDIO_UNDEFINED_INTERFACE 0x00
#define AUDIO_CONTROL_INTERFACE 0x01
#define AUDIO_STREAMING_INTERFACE 0x02
#define MIDI_STREAMING_INTERFACE 0x03

// CDC_CONTROL
#define CDC_NO_SPECIFIC_INTERFACE 0x00
#define ABSTRACT_CONTROL_MODELI_INTERFACE 0x01
#define DIRECT_LINE_MODEL_INTERFACE 0x02
#define TELEPHONE_CONTROL_MODEL_INTERFACE 0x03
#define MULTI_CHANNEL_CONTROL_MODEL_INTERFACE 0x04
#define CAPI_CONTROL_MODEL_INTERFACE 0x05
#define ETHERNET_NETWORKING_CONTROL_MODEL_INTERFACE 0x06
#define ATM_NETWORKING_CONTROL_MODEL_INTERFACE 0x07
#define WIRELESS_HANDSET_CONTROL_MODEL_INTERFACE 0x08
#define DEVICE_MANAGEMENT_INTERFACE 0x09
#define MOBILE_DIRECT_LINE_MODEL_INTERFACE 0x0A
#define OBEX_INTERFACE 0x0B
#define ETHERNET_EMULATION_MODEL_INTERFACE 0x0C
#define NETWORK_CONTROL_MODEL_INTERFACE 0x0D

// HID
#define HID_NO_SUBCLASS_INTERFACE 0x00
#define BOOT_INTERFACE_INTERFACE 0x01

// PID
#define PID_UNDEFINDED_INTERFACE 0x00
#define JOYSTICK_INTERFACE 0x01
#define GAMEPAD_INTERFACE 0x02
#define REMOTE_CONTROL_INTERFACE 0x03
#define SENSING_DEVICE_INTERFACE 0x04
#define DIGIT_TABLET_INTERFACE 0x05
#define CARD_READER_INTERFACE 0x06
#define DIGIT_PEN_INTERFACE 0x07
#define BARCODE_SCANNER_INTERFACE 0x08

// Image
#define STILL_IMAGE_CAPTURE_INTERFACE 0x01
#define SCANNER_INTERFACE 0x02
#define PRINTER_INTERFACE_SUB_CLASS 0x03
#define IMAGE_PRINTER_INTERFACE 0x04
#define MASS_STORAGE_INTERFACE_SUB_CLASS 0x05
#define SFCP_INTERFACE 0x06

// Printer
#define PRINTER_UNDEFINED_INTERFACE 0x00
#define PRINTER_INTERFACE_ 0x01
#define SCANNER_INTERFACE_ 0x02
#define PRINTER_X_SCANNER_INTERFACE 0x03

// Hub
#define FULL_SPEED_INTERFACE 0x00
#define STT_INTERFACE 0x01
#define TT_INTERFACE 0x02
#define MULTI_TT_INTERFACE 0x03

// Mass Storage
#define CONTROL_BULK_INTERRUPT_INTERFACE 0x00
#define CONTROL_BULK_INTERRUPT_W_INTR_INTERFACE 0x01
#define BULK_ONLY_INTERFACE 0x02

// SMART CARD 
#define SMART_CARD_UNDEFINED_INTERFACE 0x00
#define CHIP_SMART_CARD_INTERFACE_INTERFACE 0x01

// content security
#define CONTENT_SEC_UNDEFINED_INTERFACE 0x00
#define DRM_RM_INTERFACE 0x01
#define ENV_ENCRYPT_INTERFACE 0x10

// video
#define VIDEO_UNDEFINED_INTERFACE 0x00
#define VIDEO_CONTROL_INTERFACE 0x01
#define VIDEO_STREAMING_INTERFACE 0x02

// wireless
#define BLUETOOTH_PROGRAMMING_INTERFACE 0x01

// miscellaneous
#define COMMON_CLASS_INTERFACE 0x01
#define INTERFACE_ASSOCIATION_DESC_INTERFACE 0x02
#define WIRE_ADAPTER_MULTI_USE_TERMINAL_INTERFACE 0x03


/*****************************
 * Interface Protocols
******************************/

#define INTERFACE_PROTOCOL_KDB 0x01
#define INTERFACE_PROTOCOL_MOUSE 0x02
#define PR_PROTOCOL_UNDEFINED = 0x00

#endif