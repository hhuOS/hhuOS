#ifndef UsbService_C__include
#define UsbService_C__include

#include "../../device/usb/controller/UsbController.h"
#include "../../device/usb/driver/UsbDriver.h"
#include "../../device/usb/events/EventDispatcher.h"
#include "../../device/usb/interfaces/LoggerInterface.h"
#include "../../device/usb/interfaces/MapInterface.h"
#include "../../device/usb/interfaces/SystemInterface.h"
#include "../../device/usb/events/event/Event.h"
#include "stdint.h"
#include "../../device/usb/utility/Utils.h"

#define UHCI
// #define OHCI
// #define EHCI
// #define XHCI

#define __FOR_EACH_PCI_DEVICE__(index) \
  for (PciDevice_Struct* pci_device = pci_c->pci_devices + index; index < pci_c->devices_length; index++)

#define __INIT_SERVICE__(usb_service_c) \
  __ENTRY__(usb_service_c, head.l_e) = 0; \
  __ENTRY__(usb_service_c, add_driver_c) = &add_driver_c; \
  __ENTRY__(usb_service_c, remove_driver_c) = &remove_driver_c; \
  __ENTRY__(usb_service_c, init_uhci_routine) = &init_uhci_routine; \
  __ENTRY__(usb_service_c, init_xhci_routine) = &init_xhci_routine; \
  __ENTRY__(usb_service_c, init_ehci_routine) = &init_ehci_routine; \
  __ENTRY__(usb_service_c, init_ohci_routine) = &init_ohci_routine; \
  __ENTRY__(usb_service_c, find_controller) = &find_controller; \
  __ENTRY__(usb_service_c, submit_bulk_transfer_c) = &submit_bulk_transfer_c; \
  __ENTRY__(usb_service_c, submit_interrupt_transfer_c) = &submit_interrupt_transfer_c; \
  __ENTRY__(usb_service_c, submit_control_transfer_c) = &submit_control_transfer_c; \
  __ENTRY__(usb_service_c, register_callback_c) = &register_callback_c; \
  __ENTRY__(usb_service_c, deregister_callback_c) = &deregister_callback_c; \
  __ENTRY__(usb_service_c, register_listener_c) = &register_listener_c; \
  __ENTRY__(usb_service_c, deregister_listener_c) = &deregister_listener_c; \
  __ENTRY__(usb_service_c, get_controller) = &get_controller; \
  __ENTRY__(usb_service_c, get_dev) = &get_dev; \
  __ENTRY__(usb_service_c, get_driver) = &get_driver

struct UsbService_C {
  void (*new_service)(struct UsbService_C *usb_service_c);
  int (*add_driver_c)(struct UsbService_C *usb_service_c, UsbDriver *driver);
  int (*remove_driver_c)(struct UsbService_C *usb_service_c, UsbDriver *driver);

  void (*init_uhci_routine)(struct UsbService_C *usb_service_c, Pci_C *pci_c);
  void (*init_xhci_routine)(struct UsbService_C *usb_service_c, Pci_C *pci_c);
  void (*init_ehci_routine)(struct UsbService_C *usb_service_c, Pci_C *pci_c);
  void (*init_ohci_routine)(struct UsbService_C *usb_service_c, Pci_C *pci_c);

  UsbController *(*find_controller)(struct UsbService_C *usb_service_c,
                                    Interface *interface);

  void (*submit_bulk_transfer_c)(struct UsbService_C *usb_service_c,
                                 Interface *interface, unsigned int pipe,
                                 uint8_t prio, void *data, unsigned int len,
                                 callback_function callback);
  void (*submit_interrupt_transfer_c)(struct UsbService_C *usb_service_c,
                                      Interface *interface, unsigned int pipe,
                                      uint8_t prio, uint16_t interval,
                                      void *data, unsigned int len,
                                      callback_function callback);
  void (*submit_control_transfer_c)(struct UsbService_C *usb_service_c,
                                    Interface *interface, unsigned int pipe,
                                    uint8_t prio, void *data, uint8_t *setup,
                                    callback_function callback);
  int (*register_callback_c)(struct UsbService_C *usb_service_c,
                             uint16_t register_type, event_callback event_c);
  int (*deregister_callback_c)(struct UsbService_C *usb_service_c,
                               uint16_t register_type, event_callback event_c);

  int (*register_listener_c)(struct UsbService_C *usb_service_c,
                             EventListener *listener);
  int (*deregister_listener_c)(struct UsbService_C *usb_service_c, int id);

  void (*list_usb)(struct UsbService_C *usb_service_c);

  UsbController *(*get_controller)(struct UsbService_C *usb_service_c,
                                   list_element *l_e);
  UsbDriver* (*get_driver)(struct UsbService_C* usb_service_c, list_element* l_e);
  UsbDev* (*get_dev)(struct UsbService_C* usb_service_c, list_element* l_e);

  SystemService_C *mem_service;
  //SystemService_C *interrupt_service;

  list_head head; // controller linkage
};

typedef struct UsbService_C UsbService_C;

#ifdef __cplusplus
extern "C" {
#endif

void new_usb_service(UsbService_C *usb_service_c);

#ifdef __cplusplus
}
#endif

#endif