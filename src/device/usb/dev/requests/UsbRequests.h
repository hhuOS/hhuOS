#ifndef USB_REQUEST__INCLUDE
#define USB_REQUEST__INCLUDE

#include "stdint.h"

/*enum Device_Request {
    bmRequestType = 0x00,
    bRequest      = 0x01,
    wValue        = 0x02,
    wIndex        = 0x04,
    wLength       = 0x06
};*/

#define HOST_TO_DEVICE 0x00
#define DEVICE_TO_HOST 0x80
#define TRANSFER_DIRECTION_MASK 0x80

#define TYPE_REQUEST_STANDARD 0x00
#define TYPE_REQUEST_CLASS 0x20
#define TYPE_REQUEST_VENDOR 0x40
#define TYPE_REQUEST_MASK 0x60

#define RECIPIENT_DEVICE 0x00
#define RECIPIENT_INTERFACE 0x01
#define RECIPIENT_ENDPOINT 0x02
#define RECIPIENT_OTHER 0x03
#define RECIPIENT_MASK 0x1F

#define USE_BOOT_PROTOCOL 0x00
#define USE_REPORT_PROTOCOL 0x01

#define REQUEST_OUTPUT 0x02
#define REQUEST_INPUT  0x01

#define LOCAL_POWER 0x01 << 6
#define BUS_POWER 0x00 << 6
#define SUPPORTED_REMOTE_WAKE_UP 0x01 << 5
#define NOT_SUPPORTED_REMOTE_WAKE_UP 0x00 << 5
#define POWERED_MASK 0x40
#define REMOTE_WAKEUP_MASK 0x20

#define DIRECTION_OUT 0x00 
#define DIRECTION_IN  0x80
#define DIRECTION_MASK 0x80
#define ENDPOINT_MASK 0x0F

// only for iso
#define DATA_ENDPOINT 0x00 << 4
#define FEEDBACK_ENDPOINT 0x01 << 4
#define IMPLICIT_FEEDBACK_DATA_ENDPOINT 0x02 << 4
#define USAGE_TYPE_MASK 0x30

// only for iso
#define NO_SYNC 0x00 << 2
#define ASYNC 0x01 << 2
#define ADAPTIVE 0x02 << 2
#define SYNC 0x03 << 2
#define SYNC_TYPE_MASK 0x0C

#define TRANSFER_TYPE_CONTROL 0x00
#define TRANSFER_TYPE_ISO 0x01
#define TRANSFER_TYPE_BULK 0x02
#define TRANSFER_TYPE_INTERRUPT 0x03
#define TRANSFER_TYPE_MASK 0x03

#define WMAX_PACKET_SIZE_MASK 0x03FF

#define DEVICE_RECIPIENT_STATUS_SELF_POWERED = 0x01
#define DEVICE_RECIPIENT_STATUS_REMOTE_WAKEUP = 0x02
#define DEVICE_RECIPIENT_STATUS_MASK = 0x03

#define INTERFACE_RECIPIENT_STATUS_MASK = 0x00

#define ENDPOINT_RECIPIENT_STATUS_HALT = 0x01
#define ENDPOINT_RECIPIENT_STATUS_MASK = 0x01

struct UsbDeviceRequest {
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} __attribute__((packed));

enum BRequest_Standard {
    GET_STATUS = 0x00,
    CLEAR_FEATURE = 0x01,
    SET_FEATURE = 0x03,
    SET_ADDRESS = 0x05,
    GET_DESCRIPTOR = 0x06,
    SET_DESCRIPTOR = 0x07,
    GET_CONFIGURATION = 0x08,
    SET_CONFIGURATION = 0x09,
    GET_INTERFACE = 0x0A,
    SET_INTERFACE = 0x0B,
    SYNC_FRAME = 0x0C
};

enum BRequest_Class_HID{
    GET_REPORT = 0x01,
    SET_REPORT = 0x09,
    GET_PROTOCOL = 0x03,
    SET_PROTOCOL = 0x0B,
    SET_IDLE = 0x0A
};

enum BRequest_Class_AUDIO{
    SET_CURRENT = 0x01,
    GET_CURRENT = 0x81,
    SET_MIN = 0x02,
    GET_MIN = 0x82,
    SET_MAX = 0x03,
    GET_MAX = 0x83,
    SET_RES = 0x04,
    GET_RES = 0x84
};

enum BRequest_Class_MASS_STORAGE{
    BULK_ONLY_MASS_STORAGE_RESET = 0xFF,
    GET_MAX_LOGIC_UNIT_NUMBER    = 0xFE
};

enum BRequest_Class_PRINTER{
    GET_DEVICE_ID = 0x00,
    SET_PORT_FEATURE = 0x04,
    GET_PORT_STATUS = 0x05
};

#endif