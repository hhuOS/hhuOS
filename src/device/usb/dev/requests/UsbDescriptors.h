#ifndef USB_DEV_DESCRIPTORS__INCLUDE
#define USB_DEV_DESCRIPTORS__INCLUDE

#include "stdint.h"

enum Descriptor_Type {
  DEVICE = 0x01,
  CONFIGURATION = 0x02,
  STRING = 0x03,
  INTERFACE = 0x04,
  ENDPOINT = 0x05,
  DEVICE_QUALIFIER = 0x06,
  OTHER_SPEED_CONFIG = 0x07,
  INTERFACE_POWER = 0x08,
  HID = 0x21,
  REPORT = 0x22
};

struct DeviceDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB; // Version der Usb-Spezifikation
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice; // gerätedefinierte Version
  uint8_t iManufacturer;
  uint8_t iProduct;
  uint8_t iSerialNumber;
  uint8_t bNumConfigurations;
} __attribute__((packed));

struct DeviceQualifierDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t bcdUSB;
  uint8_t bDeviceClass;
  uint8_t bDeviceSubClass;
  uint8_t bDeviceProtocol;
  uint8_t bMaxPacketSize0;
  uint8_t bNumConfigurations;
} __attribute__((packed));

struct ConfigurationDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint16_t wTotalLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t bMaxPower;
} __attribute__((packed));

struct InterfaceDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} __attribute__((packed));

struct EndpointDescriptor {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bEndpointAddress;
  uint8_t bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
} __attribute__((packed));

struct ReportDescriptor { // used for HID
  uint8_t length;
  uint8_t type;
  uint16_t release;
  uint8_t country_code;
  uint8_t num_desc;
  uint8_t desc_type;
  uint16_t desc_len;
} __attribute__((packed));

typedef enum Descriptor_Type Descriptor_Type;
typedef struct UsbDeviceRequest UsbDeviceRequest;
typedef struct DeviceDescriptor DeviceDescriptor;
typedef struct DeviceQualifierDescriptor DeviceQualifierDescriptor;
typedef struct ConfigurationDescriptor ConfigurationDescriptor;
typedef struct InterfaceDescriptor InterfaceDescriptor;
typedef struct EndpointDescriptor EndpointDescriptor;
typedef struct ReportDescriptor ReportDescriptor;

#endif