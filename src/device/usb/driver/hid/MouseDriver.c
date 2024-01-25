#include "MouseDriver.h"
#include "../../events/event/Event.h"
#include "../../../../lib/util/io/key/InputEvents.h"
#include "../../events/event/hid/MouseEvent.h"
#include "../../include/UsbControllerInclude.h"
#include "../../controller/UsbControllerFlags.h"
#include "../../dev/UsbDevice.h"
#include "../../dev/requests/UsbRequests.h"
#include "../../include/UsbErrors.h"
#include "../../interfaces/SystemInterface.h"
#include "../../utility/Utils.h"
#include "../UsbDriver.h"

static MouseDriver* internal_driver = 0;

static void new_usb_driver(UsbDriver *usb_driver, char *name,
                           UsbDevice_ID *entry);

static void new_usb_driver(UsbDriver *usb_driver, char *name,
                           UsbDevice_ID *entry) {
  usb_driver->probe = &probe_mouse;
  usb_driver->disconnect = &disconnect_mouse;

  usb_driver->name = name;
  usb_driver->entry = entry;
  usb_driver->head.l_e = 0;
  usb_driver->l_e .l_e = 0;
}

int16_t probe_mouse(UsbDev *dev, Interface *interface) {
  Endpoint **endpoints = interface->active_interface->endpoints;
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;
  int e = interface_desc.bNumEndpoints;

  if(internal_driver == (void*)0) return -1;

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
    if (!internal_driver->dev.endpoint_addr) {
      uint8_t *mouse_buffer = (uint8_t *)mem_service->mapIO(
          mem_service, sizeof(uint8_t) * MOUSE_BUFFER_SIZE, 1);
      internal_driver->dev.endpoint_addr =
          endpoints[i]->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;
      internal_driver->dev.usb_dev = dev;
      internal_driver->dev.buffer = mouse_buffer;
      internal_driver->dev.buffer_size = MOUSE_BUFFER_SIZE;
      internal_driver->dev.priority = PRIORITY_8;
      internal_driver->dev.interface = interface;
      internal_driver->dev.interval = endpoints[i]->endpoint_desc.bInterval;
      return 1;
    }
  }
  return -1;
}

void disconnect_mouse(UsbDev *dev, Interface *interface) {}

void new_mouse_driver(MouseDriver *driver, char *name,
                      UsbDevice_ID *entry) {                        
  
  for(int i = 0; i < MOUSE_LOOK_UP_SIZE; i++){
    driver->look_up_buffer[i] = 0;
    driver->movement_buffer[i] = 0;
  }
  driver->movement_submitted = 0;

  driver->constructEvent_mouse = &constructEvent_mouse;
  driver->trigger_mouse_event = &trigger_mouse_event;
  driver->look_for_mouse_released = &look_for_mouse_released;
  driver->look_for_mouse_events = &look_for_mouse_events;
  driver->map_mouse_to_input_event = &map_mouse_to_input_event;

  internal_driver = driver;
  internal_driver->dev.endpoint_addr = 0;
  internal_driver->dev.usb_dev = 0;
  internal_driver->dev.buffer = 0;
  internal_driver->dev.buffer_size = 0;
  internal_driver->dev.priority = 0;
  internal_driver->dev.interface = 0;
  internal_driver->dev.interval = 0;
  internal_driver->dev.callback = &callback_mouse;
  internal_driver->dev.usb_driver = (UsbDriver*)driver;

  driver->super.new_usb_driver = &new_usb_driver;
  driver->super.new_usb_driver(&driver->super, name, entry);
}

// logic ; important  : check for status to process data pointer !
void callback_mouse(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER) {
    return;
  }

  uint8_t button1, button2, button3;
  uint8_t *mouse_buffer = (uint8_t *)data;

  uint8_t byte_1 = (*mouse_buffer) & 0x07;
  button1 = byte_1 & 0x01;
  button2 = byte_1 & 0x02;
  button3 = byte_1 & 0x04;

  int8_t x = *(mouse_buffer + 1);
  int8_t y = *(mouse_buffer + 2);
  int8_t z = *(mouse_buffer + 3);

  internal_driver->movement_submitted = 0;

  internal_driver->look_for_mouse_events(internal_driver, &button1, 0, &x, &y, &z);
  internal_driver->look_for_mouse_events(internal_driver, &button2, 1, &x, &y, &z);
  internal_driver->look_for_mouse_events(internal_driver, &button3, 2, &x, &y, &z);

  internal_driver->look_for_mouse_released(internal_driver, &button1, 0, &x, &y, &z);
  internal_driver->look_for_mouse_released(internal_driver, &button2, 1, &x, &y, &z);
  internal_driver->look_for_mouse_released(internal_driver, &button3, 2, &x, &y, &z);

  internal_driver->look_up_buffer[0] = button1;
  internal_driver->look_up_buffer[1] = button2 >> 1;
  internal_driver->look_up_buffer[2] = button3 >> 2;

  internal_driver->movement_buffer[0] = x;
  internal_driver->movement_buffer[1] = y;
  internal_driver->movement_buffer[2] = z;
}

void look_for_mouse_events(MouseDriver* mouse_driver, uint8_t *mouse_code, int index,
                           int8_t *x, int8_t *y, int8_t *z) {
  uint16_t event_type = MOUSE_EVENT;
  uint16_t event_value = 0;
  MouseEvent event;

  if (((*mouse_code >> index) == 1) && (mouse_driver->look_up_buffer[index] == 1)) {
    event_value = MOUSE_HOLD;
    mouse_driver->movement_submitted = 1;
    event = mouse_driver->constructEvent_mouse(mouse_driver, index, &event_value, &event_type, x, y, z);
  } else if (((*mouse_code >> index) == 1) && (mouse_driver->look_up_buffer[index] == 0)) {
    event_value = MOUSE_ENTERED;
    mouse_driver->movement_submitted = 1;
    event = mouse_driver->constructEvent_mouse(mouse_driver, index, &event_value, &event_type, x, y, z);
  }
  else if(*mouse_code == 0){
    if((*x == 0) && (*y == 0) && (*z == 0)) return;
    if((mouse_driver->movement_buffer[0] != *x) || (mouse_driver->movement_buffer[1] != *y) || (mouse_driver->movement_buffer[2] != *z)){
      if(mouse_driver->movement_submitted) return;
      event = mouse_driver->constructEvent_mouse(mouse_driver, -1, &event_value, &event_type, x ,y ,z);
      mouse_driver->movement_submitted = 1;
    }
    else return;
  }
  mouse_driver->trigger_mouse_event(mouse_driver, (GenericEvent *)&event);
}

void look_for_mouse_released(MouseDriver* mouse_driver, uint8_t *mouse_code, int index,
                             int8_t *x, int8_t *y, int8_t *z) {
  uint16_t event_type = MOUSE_EVENT;
  uint16_t event_value = MOUSE_RELEASED;
  MouseEvent event;

  int is_released = 0;
  if (*mouse_code == 0 && (mouse_driver->look_up_buffer[index] == 1)) {
    is_released = 1;
  }
  if (is_released) {
    event = mouse_driver->constructEvent_mouse(mouse_driver, index, &event_value, &event_type, x, y, z);
    mouse_driver->trigger_mouse_event(mouse_driver, (GenericEvent *)&event);
  }
}

MouseEvent constructEvent_mouse(MouseDriver* mouse_driver, int index, uint16_t *value, uint16_t *type,
                                int8_t *x, int8_t *y, int8_t *z) {

  MouseEvent event;
  uint16_t input_key = mouse_driver->map_mouse_to_input_event(mouse_driver, index);

  event.x_displacement = *x;
  event.y_displacement = *y;
  event.z_displacement = *z;
  event.super.event_code = input_key;
  event.super.event_type = *type;
  event.super.event_value = *value;

  return event;
}

void trigger_mouse_event(MouseDriver *mouse_driver, GenericEvent *event) {
  ((UsbDriver*)mouse_driver)->dispatcher->publish_event(((UsbDriver*)mouse_driver)->dispatcher, event,
                                          ((UsbDriver*)mouse_driver)->listener_id);
}

uint16_t map_mouse_to_input_event(MouseDriver* mouse_driver, int index) {
  uint16_t input_key = 0;

  if (index == RAW_BUTTON_1) {
    input_key = MOUSE_LEFT;
  } else if (index == RAW_BUTTON_2) {
    input_key = MOUSE_RIGHT;
  } else if (index == RAW_BUTTON_3) {
    input_key = MOUSE_SCROLL;
  }

  return input_key;
}

uint8_t RAW_BUTTON_1 = 0;
uint8_t RAW_BUTTON_2 = 1;
uint8_t RAW_BUTTON_3 = 2;