#include "UsbService_C.h"
#include "../../device/usb/controller/UsbController.h"
#include "../../device/usb/dev/UsbDevice.h"
#include "../../device/usb/driver/hid/KeyBoardDriver.h"
#include "../../device/usb/driver/hid/MouseDriver.h"
#include "../../device/usb/driver/UsbDriver.h"
#include "../../device/usb/events/EventDispatcher.h"
#include "../../device/usb/events/listeners/EventListener.h"
#include "../../device/usb/events/listeners/hid/KeyBoardListener.h"
#include "../../device/usb/events/listeners/hid/MouseListener.h"
#include "../../device/usb/interfaces/LoggerInterface.h"
#include "../../device/usb/interfaces/PciDeviceInterface.h"
#include "../../device/usb/interfaces/PciInterface.h"
#include "../../device/usb/interfaces/SystemInterface.h"
#include "../../device/usb/utility/Utils.h"
#include "../../device/usb/include/UsbControllerInclude.h"
#include "../../device/usb/controller/uhci/UHCI.h"
#include "stdint.h"

void new_usb_service(UsbService_C *usb_service_c) {

  #if defined(UHCI)
  Pci_C *pci_uhci = search_c_controllers(CLASS_ID, SUBCLASS_ID, INTERFACE_ID);
  usb_service_c->init_uhci_routine = &init_uhci_routine;
  usb_service_c->init_uhci_routine(usb_service_c, pci_uhci);
  #endif
  #if defined(OHCI)
  Pci_C *pci_ohci = search_c_controllers(0, 0, 0);
  usb_service_c->init_ohci_routine = &init_ohci_routine;
  usb_service_c->init_ohci_routine(usb_service_c, pci_ohci);
  #endif
  #if defined(EHCI)
  Pci_C *pci_ehci = search_c_controllers(0, 0, 0);
  usb_service_c->init_ehci_routine = &init_ehci_routine;
  usb_service_c->init_ehci_routine(usb_service_c, pci_ehci);
  #endif
  #if defined(XHCI)
  Pci_C *pci_xhci = search_c_controllers(0, 0, 0);
  usb_service_c->init_xhci_routine = &init_xhci_routine;
  usb_service_c->init_xhci_routine(usb_service_c, pci_xhci);
  #endif
}

void init_xhci_routine(UsbService_C *usb_service_c, Pci_C *pci_c) { return; }

void init_uhci_routine(UsbService_C *usb_service_c, Pci_C *pci_c) {
  if (pci_c == (void *)0)
    return;

  usb_service_c->head.l_e = 0;
  usb_service_c->request_mem_service = &request_mem_service;
  usb_service_c->add_driver_c = &add_driver_c;
  usb_service_c->remove_driver_c = &remove_driver_c;
  usb_service_c->init_uhci_routine = &init_uhci_routine;
  usb_service_c->init_xhci_routine = &init_xhci_routine;
  usb_service_c->init_ehci_routine = &init_ehci_routine;
  usb_service_c->init_ohci_routine = &init_ohci_routine;
  usb_service_c->find_controller = &find_controller;
  usb_service_c->submit_bulk_transfer_c = &submit_bulk_transfer_c;
  usb_service_c->submit_interrupt_transfer_c = &submit_interrupt_transfer_c;
  usb_service_c->submit_control_transfer_c = &submit_control_transfer_c;
  usb_service_c->register_callback_c = &register_callback_c;
  usb_service_c->deregister_callback_c = &deregister_callback_c;
  usb_service_c->register_listener_c = &register_listener_c;
  usb_service_c->deregister_listener_c = &deregister_listener_c;
  usb_service_c->get_controller = &get_controller;
  usb_service_c->get_dev = &get_dev;
  usb_service_c->get_driver = &get_driver;
  //usb_service_c->request_interrupt_service = &request_interrupt_service;
  SystemService_C *mem_service =
      usb_service_c->request_mem_service(usb_service_c);
  usb_service_c->mem_service = mem_service;

  _UHCI *controller;

  MemoryService_C* m = (MemoryService_C*)container_of(mem_service, MemoryService_C, super);

  int i = 0;
  for (PciDevice_Struct* pci_device = pci_c->pci_devices + i; i < pci_c->devices_length; i++) {
    controller = (_UHCI*)m->allocateKernelMemory_c(m, sizeof(_UHCI), 0);
    controller->new_UHCI = &new_UHCI;
    controller->new_UHCI(controller, pci_device, mem_service);

    if (usb_service_c->head.l_e == (void *)0) {
      usb_service_c->head.l_e = &controller->super.l_e;
    } else {
      list_element *l_e = usb_service_c->head.l_e;
      while (l_e->l_e != (void *)0) {
        l_e = l_e->l_e;
      }
      l_e->l_e = &controller->super.l_e;
    }
  }
  m->freeKernelMemory_c(m, pci_c, 0);  
}

void init_ohci_routine(UsbService_C *usb_service_c, Pci_C *pci_c) { return; }

void init_ehci_routine(UsbService_C *usb_service_c, Pci_C *pci_c) { return; }

// 1 if no errors in any controller
// -1 error occur at min. 1
int add_driver_c(UsbService_C *usb_service_c, UsbDriver *driver) {
  list_element *l_e = usb_service_c->head.l_e;
  int status = 0;
  while (l_e != (void *)0) {
    UsbController *controller =
        (UsbController *)container_of(l_e, UsbController, l_e);
    int s = controller->register_driver(controller, driver);
    if (s == 1 && status <= 0)
      status = s;
    else if (s == -1 && status <= 0)
      status = -1;
    l_e = l_e->l_e;
  };

  return status;
}

// same as above
int remove_driver_c(UsbService_C *usb_service_c, UsbDriver *driver) {
  list_element *l_e = usb_service_c->head.l_e;
  int status;
  while (l_e != (void *)0) {
    UsbController *controller =
        (UsbController *)container_of(l_e, UsbController, l_e);
    int s = controller->remove_driver(controller, driver);
    if (s == 1 && status <= 0)
      status = s;
    else if (s == -1 && status <= 0)
      status = -1;
    l_e = l_e->l_e;
  }

  return status;
}

void submit_bulk_transfer_c(UsbService_C *usb_service_c, Interface *interface,
                            unsigned int pipe, uint8_t prio, void *data,
                            unsigned int len, callback_function callback) {
  UsbController *usb_controller =
      usb_service_c->find_controller(usb_service_c, interface);
  usb_controller->bulk(usb_controller, interface, pipe, prio, data, len,
                       callback);
}

void submit_interrupt_transfer_c(UsbService_C *usb_service_c,
                                 Interface *interface, unsigned int pipe,
                                 uint8_t prio, uint16_t interval, void *data,
                                 unsigned int len, callback_function callback) {
  UsbController *usb_controller =
      usb_service_c->find_controller(usb_service_c, interface);
  usb_controller->interrupt(usb_controller, interface, pipe, prio, data, len,
                            interval, callback);
}

void submit_control_transfer_c(UsbService_C *usb_service_c,
                               Interface *interface, unsigned int pipe,
                               uint8_t prio, void *data, uint8_t *setup,
                               callback_function callback) {
  UsbController *usb_controller =
      usb_service_c->find_controller(usb_service_c, interface);
  usb_controller->control(usb_controller, interface, pipe, prio, data, setup,
                          callback);
}

UsbController *find_controller(UsbService_C *usb_service_c,
                               Interface *interface) {
  list_element *l_e = usb_service_c->head.l_e;
  while (l_e != (void *)0) {
    UsbController *controller =
        (UsbController *)container_of(l_e, UsbController, l_e);
    if (controller->contains_interface(controller, interface)) {
      return controller;
    }
    l_e = l_e->l_e;
  }
  return (void *)0;
}

SystemService_C *request_mem_service(UsbService_C *usb_service_c) {
  MemoryService_C *mem_service = new_mem_service();

  return (SystemService_C *)mem_service;
}

UsbController *get_controller(UsbService_C *usb_service_c, list_element *l_e) {
  return (UsbController *)container_of(l_e, UsbController, l_e);
}

UsbDev* get_dev(UsbService_C* usb_service_c, list_element* l_e){
  return (UsbDev*)container_of(l_e, UsbDev, l_e);
}

UsbDriver* get_driver(UsbService_C* usb_service_c, list_element* l_e){
  return (UsbDriver*)container_of(l_e, UsbDriver, l_e);
}

// < 0 error occured in min. 1 controller
int register_callback_c(UsbService_C *usb_service_c, uint16_t register_type,
                        event_callback event_c) {
  int status = 0;
  list_element *l_e = usb_service_c->head.l_e;
  while (l_e != 0) {
    UsbController *controller =
        (UsbController *)container_of(l_e, UsbController, l_e);
    int s = controller->insert_callback(controller, register_type, event_c);
    if (s == 1 && status <= 0)
      status = s;
    else if (s == -1 && status <= 0)
      status = -1;
    l_e = l_e->l_e;
  }

  return status;
}

int deregister_callback_c(UsbService_C *usb_service_c, uint16_t register_type,
                          event_callback event_c) {
  int status = 0;
  list_element *l_e = usb_service_c->head.l_e;
  while (l_e != 0) {
    UsbController *controller =
        (UsbController *)container_of(l_e, UsbController, l_e);
    int s = controller->delete_callback(controller, register_type, event_c);
    if (s == 1 && status <= 0)
      status = s;
    else if (s == -1 && status <= 0)
      status = -1;
    l_e = l_e->l_e;
  }

  return status;
}

int register_listener_c(UsbService_C *usb_service_c, EventListener *listener) {
  int status = 0;
  list_element *l_e = usb_service_c->head.l_e;
  while (l_e != 0) {
    UsbController *controller =
        (UsbController *)container_of(l_e, UsbController, l_e);
    int s = controller->insert_listener(controller, listener);
    if (s != -1 && status <= 0)
      status = s;
    else if (s == -1 && status <= 0)
      status = -1;
    l_e = l_e->l_e;
  }

  return status;
}

int deregister_listener_c(UsbService_C *usb_service_c, int id) {
  int status = 0;
  list_element *l_e = usb_service_c->head.l_e;
  while (l_e != 0) {
    UsbController *controller =
        (UsbController *)container_of(l_e, UsbController, l_e);
    int s = controller->delete_listener(controller, id);
    if (s == 1 && status <= 0)
      status = s;
    else if (s == -1 && status <= 0)
      status = -1;
    l_e = l_e->l_e;
  }

  return status;
}