#include "UsbGeneral.h"
#include "UsbInterface.h"
#include "../dev/UsbDevice.h"
#include "../dev/requests/UsbRequests.h"
#include "../dev/requests/UsbDescriptors.h"
#include "../controller/UsbControllerFlags.h"

uint8_t* get_descriptor_setup(uint8_t descriptor_type , uint8_t descriptor_index, uint16_t len){
    if(descriptor_type != DEVICE && descriptor_type != CONFIGURATION && descriptor_type != STRING
      && descriptor_type != INTERFACE && descriptor_type != ENDPOINT){
        return (void*)0;
    }
    
    uint8_t* request = (uint8_t*)interface_allocateMemory(sizeof(uint8_t) * USB_REQUEST_SIZE, 0);
    
    uint8_t low_part = (uint8_t)((len >> 0) & 0xFF);
    uint8_t high_part = (uint8_t)((len >> 8) & 0xFF);

    *request = DEVICE_TO_HOST;
    *(request+1) = GET_DESCRIPTOR;
    *(request+2) = descriptor_type << 8;
    *(request+3) = descriptor_index;
    *(request+4) = 0;
    *(request+5) = 0;
    *(request+6) = high_part;
    *(request+7) = low_part;

    return request;
}

uint8_t* get_configuration_setup(){
    uint8_t* request = (uint8_t*)interface_allocateMemory(sizeof(uint8_t) * USB_REQUEST_SIZE, 0);

    *request = DEVICE_TO_HOST;
    *(request+1) = GET_CONFIGURATION;
    *(request+2) = 0;
    *(request+3) = 0;
    *(request+4) = 0;
    *(request+5) = 0;
    *(request+6) = 0;
    *(request+7) = 1;

    return request;
}

uint8_t* get_interface_setup(UsbDev* dev, uint16_t interface_index){
    if(dev->active_config->config_desc.bNumInterfaces >= interface_index){
        return (void*)0;
    }

    uint8_t* request = (uint8_t*)interface_allocateMemory(sizeof(uint8_t) * USB_REQUEST_SIZE, 0);

    uint8_t low_part  = (uint8_t)((interface_index >> 0) & 0xFF);
    uint8_t high_part = (uint8_t)((interface_index >> 8) & 0xFF);

    *request = DEVICE_TO_HOST | RECIPIENT_INTERFACE;
    *(request+1) = GET_INTERFACE;
    *(request+2) = 0;
    *(request+3) = 0;
    *(request+4) = high_part;
    *(request+5) = low_part;
    *(request+6) = 0;
    *(request+7) = 1;

    return request;
}

uint8_t *set_interface_setup(UsbDev* dev, unsigned int alternate_setting, 
                             unsigned int interface_index){
    if(dev->active_config->config_desc.bNumInterfaces < interface_index){
        return (void*)0;
    }

    int count = 0;
    Alternate_Interface* alt_interface = dev->active_config->interfaces[interface_index]->alternate_interfaces;
    while(alt_interface != (void*)0){
        count++;
        alt_interface = alt_interface->next;
    }

    if(count < alternate_setting){
        return (void*)0;
    }

    uint8_t* request = (uint8_t*)interface_allocateMemory(sizeof(uint8_t) * USB_REQUEST_SIZE, 0);

    uint8_t low_part  = (uint8_t)((interface_index >> 0) & 0xFF);
    uint8_t high_part = (uint8_t)((interface_index >> 8) & 0xFF);

    uint8_t low_part_alt = (uint8_t)((alternate_setting >> 0) & 0xFF);
    uint8_t high_part_alt = (uint8_t)((alternate_setting >> 8) & 0xFF);

    *request = HOST_TO_DEVICE;
    *(request+1) = SET_INTERFACE;
    *(request+2) = high_part_alt;
    *(request+3) = low_part_alt;
    *(request+4) = high_part;
    *(request+5) = low_part;
    *(request+6) = 0;
    *(request+7) = 1;

    return request;
}
                             
uint8_t* get_status_setup(uint8_t recipient, uint16_t index){
    if(recipient != RECIPIENT_DEVICE && recipient != RECIPIENT_ENDPOINT && recipient != RECIPIENT_INTERFACE){
        return (void*)0;
    }

    if(recipient == RECIPIENT_DEVICE && index > 0){
        return (void*)0;
    }

    uint8_t* request = (uint8_t*)interface_allocateMemory(sizeof(uint8_t) * USB_REQUEST_SIZE, 0);

    uint8_t low_part  = (uint8_t)((index >> 0) & 0xFF);
    uint8_t high_part = (uint8_t)((index >> 8) & 0xFF);

    *request = DEVICE_TO_HOST | recipient;
    *(request+1) = GET_STATUS;
    *(request+2) = 0;
    *(request+3) = 0;
    *(request+4) = high_part;
    *(request+5) = low_part;
    *(request+6) = 0;
    *(request+7) = 2;

    return request;
}

// return type :
// bit 7 : direction
// bit 6:5 : type
// bit 4 always 0
// bit 3:0 endpoint

unsigned int usb_sndctrlpipe(unsigned int endpoint) {
  return DIRECTION_OUT_PIPE | CONTROL_PIPE | endpoint;
}

unsigned int usb_rcvctrlpipe(unsigned int endpoint) {
  return DIRECTION_IN_PIPE | CONTROL_PIPE | endpoint;
}

unsigned int usb_sndbulkpipe(unsigned int endpoint) {
  return DIRECTION_OUT_PIPE | BULK_PIPE | endpoint;
}

unsigned int usb_rcvbulkpipe(unsigned int endpoint) {
  return DIRECTION_IN_PIPE | BULK_PIPE | endpoint;
}

unsigned int usb_sndintpipe(unsigned int endpoint) {
  return DIRECTION_OUT_PIPE | INTERRUPT_PIPE | endpoint;
}

unsigned int usb_rcvintpipe(unsigned int endpoint) {
  return DIRECTION_IN_PIPE | INTERRUPT_PIPE | endpoint;
}

unsigned int usb_sndisopipe(unsigned int endpoint) {
  return DIRECTION_OUT_PIPE | ISO_PIPE | endpoint;
}

unsigned int usb_rcvisopipe(unsigned int endpoint) {
  return DIRECTION_IN_PIPE | ISO_PIPE | endpoint;
}