#include "HubDriver.h"
#include "../UsbDriver.h"
#include "../../dev/UsbDevice.h"
#include "../../include/UsbControllerInclude.h"
#include "../../include/UsbErrors.h"

static HubDriver* internal_hub_driver = 0;

static void new_usb_driver(UsbDriver *usb_driver, char *name,
                           UsbDevice_ID *entry);

static void new_usb_driver(UsbDriver *usb_driver, char *name,
                           UsbDevice_ID *entry) {
  usb_driver->probe = &probe_hub;
  usb_driver->disconnect = &disconnect_hub;

  usb_driver->name = name;
  usb_driver->entry = entry;
  usb_driver->head.l_e = 0;
  usb_driver->l_e.l_e = 0;
}

void callback_hub(UsbDev* dev, uint32_t status, void* data){

}

int16_t probe_hub(UsbDev* dev, Interface* interface){
    Endpoint **endpoints = interface->active_interface->endpoints;
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;
  // check if we should support that interface !!!
  int e = interface_desc.bNumEndpoints;

  MemoryService_C *mem_service =
      (MemoryService_C *)container_of(dev->mem_service, MemoryService_C, super);

  // select only 1 endpoint
  for (int i = 0; i < e; i++) {
    // check for type , direction
    if (!(endpoints[i]->endpoint_desc.bEndpointAddress & DIRECTION_IN)) {
      continue;
    }
    if (!(endpoints[i]->endpoint_desc.bmAttributes & TRANSFER_TYPE_INTERRUPT)) {
      continue;
    }
    if (!internal_hub_driver->dev.endpoint_addr) {
      uint8_t *key_board_buffer = (uint8_t *)mem_service->mapIO(
          mem_service, sizeof(uint8_t), 1);

      internal_hub_driver->dev.endpoint_addr =
          endpoints[i]->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;
      internal_hub_driver->dev.usb_dev = dev;
      internal_hub_driver->dev.buffer = key_board_buffer;
      internal_hub_driver->dev.buffer_size = 0;
      internal_hub_driver->dev.priority = PRIORITY_8;
      internal_hub_driver->dev.interface = interface;
      internal_hub_driver->dev.interval = endpoints[i]->endpoint_desc.bInterval;
      return 1;
    }
  }
  
  return -1;
}

void disconnect_hub(UsbDev* dev, Interface* interface){

}

void new_hub_driver(HubDriver* driver, char* name, UsbDevice_ID* entry){
    driver->super.new_usb_driver = &new_usb_driver;
    driver->super.new_usb_driver(&driver->super, name, entry);
}

void configure_callback(UsbDev* dev, uint32_t status, void* data){
    internal_hub_driver->callback_invoked = 1;
    if(status & S_TRANSFER)
        internal_hub_driver->success_transfer = 1;
    else if(status & E_TRANSFER)
        internal_hub_driver->success_transfer = 0;
}

int configure_hub(HubDriver* driver){
    UsbDev* dev = driver->dev.usb_dev;
    Interface* itf = driver->dev.interface;

    MemoryService_C* m = (MemoryService_C*)container_of(dev->mem_service, MemoryService_C, super);
    uint8_t* data = (uint8_t*)m->mapIO(m, PAGE_SIZE * sizeof(uint8_t), 1);

    if(read_hub_descriptor(driver, dev, itf, data) == -1) return -1;
    if(read_hub_status(driver, dev, itf, data) == -1) return -1;
    
}

int read_hub_status(HubDriver* driver, UsbDev* dev, Interface* itf, uint8_t* data){
    if(dev->get_status(dev, itf, data, &configure_callback) == -1)
        return -1;
    while(!driver->callback_invoked){}
    driver->callback_invoked = 0;
    if(!driver->success_transfer)
        return -1;
    driver->x_powered = *data & SELF_POWERED;
    driver->x_wakeup = (*data & REMOTE_WAKEUP) >> 1;

    return 1;
}

int read_hub_descriptor(HubDriver* driver, UsbDev* dev, Interface* itf, uint8_t* data){
    if(dev->get_descriptor(dev, itf, data, 2, &configure_callback) == -1)
        return -1;
    // instead of polling in the control block, we will just poll in here -> needed to change again the whole control transfer via flag
    while(!driver->callback_invoked){}
    driver->callback_invoked = 0;
    if(!driver->success_transfer)
        return -1;
    HubDescriptor* hub_desc = (HubDescriptor*)data;
    uint8_t hub_desc_len = hub_desc->len;
    if(dev->get_descriptor(dev, itf, data, hub_desc_len, &configure_callback) == -1)
        return -1;
    while(!driver->callback_invoked){}
    driver->callback_invoked = 0;
    if(!driver->success_transfer)
        return -1;
    driver->hub_desc = *hub_desc;

    return 1;
}