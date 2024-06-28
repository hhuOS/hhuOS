#ifndef USB_DEV_DATA__INCLUDE
#define USB_DEV_DATA__INCLUDE

#include "stdint.h"

#define STRING_BUFFER_SIZE 256
#define CONFIG_BUFFER_SIZE 512

#define LANGID_ENGLISH_US 0x0409
#define LANGID_ENGLISH_AT 0x0C09
#define LANGID_GERMAN_STD 0x0407
#define LANGID_ENGLISH_CN 0x1009
#define LANGID_FRENCH_STD 0x040C
#define LANGID_SPAIN_MEX 0x080A

#define SUPPORTED_LANGID_SIZE 10

#define LOW_SPEED 0x00
#define FULL_SPEED 0x01
#define HIGH_SPEED 0x02 // not supported 1.0

struct Endpoint {
  EndpointDescriptor endpoint_desc;
  void* class_specific;
};

struct Alternate_Interface {
  InterfaceDescriptor alternate_interface_desc;
  struct Endpoint **endpoints;

  void* class_specific;
  struct Alternate_Interface *next;
};

struct Interface {
  struct Alternate_Interface *alternate_interfaces;
  struct Alternate_Interface *active_interface;
  uint8_t active; // checks if a driver already has access to this interface

  void *driver; // UsbDriver*
  void *data;
  char *interface_description;
};

struct Configuration {
  struct ConfigurationDescriptor config_desc;
  struct Interface **interfaces;

  char *config_description;
};

typedef struct Endpoint Endpoint;
typedef struct Alternate_Interface Alternate_Interface;
typedef struct Interface Interface;
typedef struct Configuration Configuration;

extern const uint8_t DEFAULT_STATE;
extern const uint8_t ADDRESS_STATE;
extern const uint8_t CONFIGURED_STATE;

#endif