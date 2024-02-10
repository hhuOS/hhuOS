#ifndef UsbDriver__include
#define UsbDriver__include

#include "stdint.h"
#include "../events/listeners/EventListener.h"
#include "../events/EventDispatcher.h"
#include "../utility/Utils.h"
#include "../dev/UsbDevice.h"

#define MAX_DEVICES_PER_USB_DRIVER 10

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

struct UsbDriver{

    void (*new_usb_driver)(struct UsbDriver* usb_driver, char* name, struct UsbDevice_ID* entry);
    int16_t (*probe)(UsbDev* dev, Interface* interface);
    void (*disconnect)(UsbDev* dev, Interface* interface);

    char* name;
    UsbDevice_ID* entry;

    list_head head; // linking devices
    list_element l_e; // element in controller linkage

    int listener_id;

    EventDispatcher* dispatcher; // set by appropriate UsbController
};

typedef struct UsbDriver UsbDriver;

//void add_id(UsbDriver* driver, int id);
//int get_id(UsbDriver* driver);

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

#endif