/*********
 * Usb 1.0 -> UHCI
 * Usb 2.0 -> EHCI
 * Usb 3.0 -> xHCI
 *
 * Interfacing each controller ; each controller may then be implemented
 * seperatly
 **********/

#ifndef USB_INCLUDE
#define USB_INCLUDE

#include "../dev/UsbDevice.h"
#include "../dev/data/UsbDev_Data.h"
#include "../dev/requests/UsbRequests.h"
#include "../driver/UsbDriver.h"
#include "../interfaces/IoPortInterface.h"
#include "../interfaces/LoggerInterface.h"
#include "../interfaces/MapInterface.h"
#include "../interfaces/MutexInterface.h"
#include "../interfaces/PciDeviceInterface.h"
#include "../interfaces/PciInterface.h"
#include "../interfaces/SystemInterface.h"
#include "stdint.h"
#include "../utility/Utils.h"
#include "../events/EventDispatcher.h"

enum UsbControllerType {
  TYPE_UHCI = 0x01,
  TYPE_OHCI = 0x02,
  TYPE_EHCI = 0x03,
  TYPE_xHCI = 0x04
};

typedef enum UsbControllerType UsbControllerType;

struct UsbController {
  void (*new_usb_controller)(struct UsbController *usb_controller, SystemService_C* mem_service,
                             const char *name);
  void (*poll)(struct UsbController
                   *usb_controller); // runnable function passed to thread
  int (*register_driver)(struct UsbController *usb_controller,
                         UsbDriver *driver);
  int (*remove_driver)(struct UsbController *usb_controller, UsbDriver *driver);

  void (*control)(struct UsbController *controller, Interface *interface,
                  unsigned int pipe, uint8_t priority, void *data,
                  uint8_t *setup, callback_function callback);
  void (*interrupt)(struct UsbController *controller, Interface *interface,
                    unsigned int pipe, uint8_t priority, void *data,
                    unsigned int len, uint16_t interval,
                    callback_function callback);
  void (*bulk)(struct UsbController *controller, Interface *interface,
               unsigned int pipe, uint8_t priority, void *data,
               unsigned int len, callback_function callback);

  int (*contains_dev)(struct UsbController *controller, UsbDev *dev);

  int (*contains_interface)(struct UsbController *controller,
                            Interface *interface);

  UsbControllerType (*is_of_type)(struct UsbController *usb_controller);

  int (*insert_callback)(struct UsbController *controller, uint16_t reg_type,
                         event_callback callback);
  int (*delete_callback)(struct UsbController *controller, uint16_t reg_type,
                         event_callback callback);

  int (*insert_listener)(struct UsbController *controller,
                         EventListener *listener);
  int (*delete_listener)(struct UsbController *controller, int id);

  int (*supported_event_listener_type)(struct UsbController* controller, uint16_t event_type);

  uint16_t (*reset_port)(struct UsbController* controller, uint8_t port);

  void (*add_device)(struct UsbController* controller, UsbDev* dev);

  void (*link_device_to_driver)(struct UsbController* controller, UsbDev* dev, UsbDriver* driver);

  void (*link_driver_to_controller)(struct UsbController* controller, UsbDriver* driver);

  void (*link_driver_to_interface)(struct UsbController* controller, UsbDriver* driver, Interface* interface);

  void (*interrupt_entry_point)(UsbDev* dev, Endpoint* endpoint, void* data, unsigned int len, uint8_t prio, uint16_t interval, callback_function callback);

  void (*control_entry_point)(UsbDev* dev, UsbDeviceRequest* device_request, void* data, uint8_t prio, Endpoint* endpoint, callback_function callback, uint8_t flags);

  void (*bulk_entry_point)(UsbDev* dev, Endpoint* endpoint, void* data, unsigned int len, uint8_t priority, callback_function callback, uint8_t flags);

  EventDispatcher* (*request_event_dispatcher)(struct UsbController* controller, SystemService_C* mem_service);

  SuperMap* (*request_interface_device_map)(struct UsbController* controller, SystemService_C* mem_service);

  Mutex_C* (*request_mutex)(struct UsbController* controller, SystemService_C* mem_service);

  void (*runnable_function)(struct UsbController* controller);
  void (*handler_function)(struct UsbController* controller);

  const char *name;

  EventDispatcher* dispatcher;

  SuperMap* interface_dev_map;
  Mutex_C* controller_mutex;

  list_head head_dev;
  list_head head_driver;

  list_element l_e; // service linkage
};

extern const char *xHCI_name;
extern const char *EHCI_name;
extern const char *OHCI_name;
extern const char *UHCI_name;

typedef struct UsbController UsbController;
void new_super_usb_controller(UsbController *controller, SystemService_C* m, const char *name);
EventDispatcher *request_event_dispatcher(UsbController*controller, SystemService_C* m);
SuperMap* request_interface_device_map(UsbController* controller, SystemService_C* m);
Mutex_C* request_mutex(UsbController* controller, SystemService_C* mem_service);

int insert_listener(UsbController *controller, EventListener *listener);
int delete_listener(UsbController *controller, int id);

int insert_callback(UsbController *controller, uint16_t reg_type,
                    event_callback callback);
int delete_callback(UsbController *controller, uint16_t reg_type,
                    event_callback callback);

int supported_event_listener_type(UsbController *controller, uint16_t event_listener_type);

void add_device(UsbController *controller, UsbDev *dev);
int contain_device(UsbController *controller, UsbDev *dev);

extern const char *CONTROL_TRANSFER;
extern const char *BULK_TRANSFER;
extern const char *ISO_TRANSFER;
extern const char *INTERRUPT_TRANSFER;

extern const char *SETUP_TRANSACTION;
extern const char *DATA_IN_TRANSACTION;
extern const char *DATA_OUT_TRANSACTION;
extern const char *STATUS_TRANSACTION;

#endif