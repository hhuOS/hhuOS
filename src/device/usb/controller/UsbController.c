#include "UsbController.h"
#include "../interfaces/SystemInterface.h"
#include "../events/EventDispatcher.h"
#include "../interfaces/MapInterface.h"
#include "../include/UsbControllerInclude.h"

const char* xHCI_name = "xHCI";
const char* EHCI_name = "EHCI";
const char* OHCI_name = "OHCI";
const char* UHCI_name = "UHCI";

const char* CONTROL_TRANSFER = "Control";
const char* BULK_TRANSFER = "Bulk";
const char* ISO_TRANSFER = "Iso";
const char* INTERRUPT_TRANSFER = "Interrupt";

const char* SETUP_TRANSACTION = "Setup";
const char* DATA_IN_TRANSACTION = "Data_IN";
const char* DATA_OUT_TRANSACTION = "Data_OUT";
const char* STATUS_TRANSACTION = "Status";

void new_super_usb_controller(UsbController* controller, SystemService_C* mem_service, const char* name){
    controller->name = name;
    controller->l_e.l_e = 0;

    controller->head_dev.l_e = 0;
    controller->head_driver.l_e = 0;

    controller->request_event_dispatcher = &request_event_dispatcher;
    controller->request_interface_device_map = &request_interface_device_map;
    controller->supported_event_listener_type = &supported_event_listener_type;
    controller->insert_listener = &insert_listener;
    controller->delete_listener = &delete_listener;
    controller->insert_callback = &insert_callback;
    controller->delete_callback = &delete_callback;
    controller->contains_dev = &contain_device;
    controller->add_device = &add_device;
    controller->request_mutex = &request_mutex;

    controller->dispatcher = controller->request_event_dispatcher(controller, mem_service);
    controller->interface_dev_map = controller->request_interface_device_map(controller, mem_service);
    controller->controller_mutex = controller->request_mutex(controller, mem_service);
}

EventDispatcher *request_event_dispatcher(UsbController *controller, SystemService_C* mem_service){
    EventDispatcher *dispatcher;
    MemoryService_C *m = (MemoryService_C *)container_of(mem_service, MemoryService_C, super);

    dispatcher = (EventDispatcher *)m->allocateKernelMemory_c(m, sizeof(EventDispatcher), 0);
    dispatcher->new_event_dispatcher = &new_event_dispatcher;
    dispatcher->new_event_dispatcher(dispatcher);

    dispatcher->init_event_dispatcher_map(dispatcher);

    return dispatcher;
}

Mutex_C* request_mutex(UsbController* controller, SystemService_C* mem_service){
  MemoryService_C *m = (MemoryService_C *)container_of(mem_service, MemoryService_C, super);

  Mutex_C* mutex = (Mutex_C *)m->allocateKernelMemory_c(m, sizeof(Mutex_C), 0);
  mutex->new_mutex = &new_mutex;
  mutex->new_mutex(mutex);

  return mutex;
}

void add_device(UsbController *controller, UsbDev *dev) {
  controller->controller_mutex->acquire_c(controller->controller_mutex);
  if (controller->head_dev.l_e == (void *)0) {
    controller->head_dev.l_e = &dev->l_e;
  } else {
    list_element *l_e = controller->head_dev.l_e;
    while (l_e->l_e != (void *)0) {
      l_e = l_e->l_e;
    }
    l_e->l_e = &dev->l_e;
  }

  controller->controller_mutex->release_c(controller->controller_mutex);
}

int contain_device(UsbController *controller, UsbDev *dev) {
  controller->controller_mutex->acquire_c(controller->controller_mutex);
  list_element *l_e = controller->head_dev.l_e;
  while (l_e != (void *)0) {
    UsbDev *container_dev = (UsbDev *)container_of(l_e, UsbDev, l_e);
    if (container_dev == dev) {
      controller->controller_mutex->release_c(controller->controller_mutex);
      return 1;
    }
    l_e = l_e->l_e;
  }

  controller->controller_mutex->release_c(controller->controller_mutex);

  return 0;
}

SuperMap* request_interface_device_map(UsbController* controller, SystemService_C* mem_service){
    MemoryService_C *m = (MemoryService_C *)container_of(mem_service, MemoryService_C, super);

    Interface_Device_Map *interface_device_map =
        m->allocateKernelMemory_c(m, sizeof(Interface_Device_Map), 0);

    interface_device_map->new_map = &newInterface_Device_Map;
    interface_device_map->new_map(interface_device_map,
                                "Map<Interface*,UsbDev*>");
    return (SuperMap*)interface_device_map;                                
}

// -1 -> fault
// >= 0 -> id of listener in dispatcher
int insert_listener(UsbController *controller, EventListener *listener) {
  if (listener == (void *)0)
    return -1;

  return controller->dispatcher->register_event_listener(controller->dispatcher, listener);
}

int delete_listener(UsbController *controller, int id) {
  if (id < 0)
    return -1;

  return controller->dispatcher->deregister_event_listener(controller->dispatcher, id);
}

int insert_callback(UsbController *controller, uint16_t reg_type,
                    event_callback callback) {
  if (reg_type < 0)
    return -1;
  if (callback == (void *)0)
    return -1;


  if (!controller->supported_event_listener_type(controller, reg_type))
    return -1;

  return controller->dispatcher->reg_callback(controller->dispatcher, callback, reg_type);
}

int supported_event_listener_type(UsbController *controller, uint16_t event_listener_type) {
  int supported = 0;

  if (event_listener_type == MOUSE_LISTENER) {
    supported = 1;
  } else if (event_listener_type == KEY_BOARD_LISTENER) {
    supported = 1;
  }

  return supported;
}

int delete_callback(UsbController *controller, uint16_t reg_type,
                    event_callback callback) {
  if (reg_type < 0)
    return -1;
  if (callback == (void *)0)
    return -1;

  if (!controller->supported_event_listener_type(controller, reg_type))
    return -1;

  return controller->dispatcher->dereg_callback(controller->dispatcher, callback, reg_type);
}