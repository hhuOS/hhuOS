#ifndef UsbGeneral__include
#define UsbGeneral__include

#include "stdint.h"
#include "../dev/UsbDevice.h"

#define USB_REQUEST_SIZE 8

uint8_t* get_descriptor_setup(uint8_t descriptor_type , uint8_t descriptor_index, uint16_t len);

uint8_t* get_configuration_setup();

uint8_t* get_interface_setup(UsbDev* dev, uint16_t interface_index);

uint8_t *set_interface_setup(UsbDev* dev, unsigned int alternate_setting, 
                             unsigned int interface_index);

uint8_t* get_status_setup(uint8_t recipient, uint16_t index);

unsigned int usb_sndctrlpipe(unsigned int endpoint);
unsigned int usb_rcvctrlpipe(unsigned int endpoint);
unsigned int usb_sndbulkpipe(unsigned int endpoint);
unsigned int usb_rcvbulkpipe(unsigned int endpoint);
unsigned int usb_sndintpipe(unsigned int endpoint);
unsigned int usb_rcvintpipe(unsigned int endpoint);
unsigned int usb_sndisopipe(unsigned int endpoint);
unsigned int usb_rcvisopipe(unsigned int endpoint);

#endif 