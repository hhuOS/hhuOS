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
#include "components/ControllerRegister.h"

//#define __PLUG_AND_PLAY__

#ifdef __PLUG_AND_PLAY__
#define __USB_LOCK__(controller) \
  __STRUCT_CALL__(controller->controller_mutex, acquire_c)
#define __USB_RELEASE__(controller) \
  __STRUCT_CALL__(controller->controller_mutex, release_c)
#else
#define __USB_LOCK__(controller)
#define __USB_RELEASE__(controller)
#endif

#define DECLARE_CONTROLLER(name, type, mem_service, ...) \
  DECLARE_ ## type(name, mem_service, ## __VA_ARGS__)

#define __INIT_CONTROLLER__(controller, name) \
  __ENTRY__(controller, request_event_dispatcher) = &request_event_dispatcher; \
  __ENTRY__(controller, init_controller_logger) = &init_logger; \
  __ENTRY__(controller, supported_event_listener_type) = &supported_event_listener_type; \
  __ENTRY__(controller, insert_listener) = &insert_listener; \
  __ENTRY__(controller, delete_listener) = &delete_listener; \
  __ENTRY__(controller, insert_callback) = &insert_callback; \
  __ENTRY__(controller, delete_callback) = &delete_callback; \
  __ENTRY__(controller, add_device) = &add_device; \
  __ENTRY__(controller, contains_dev) = &contain_device; \
  __ENTRY__(controller, look_up) = &look_for_reg; \
  __ENTRY__(controller, register_driver) = &register_driver; \
  __ENTRY__(controller, link_device_to_driver) = &link_device_to_driver; \
  __ENTRY__(controller, link_driver_to_controller) = &link_driver_to_controller; \
  __ENTRY__(controller, link_driver_to_interface) = &link_driver_to_interface; \
  __ENTRY__(controller, remove_driver) = &deregister_driver; \
  __ENTRY__(controller, dump_drivers) = &dump_drivers; \
  __ENTRY__(controller, dump_devices) = &dump_devices; \
  __ENTRY__(controller, remove_controller_linkage) = &remove_controller_linkage; \
  __ENTRY__(controller, remove_device_linkage) = &remove_device_linkage
  
#define __UHC_CALL_LOGGER_INFO__(controller, message, ...) \
  __STRUCT_CALL__(controller->controller_logger, info_c, (message), ## __VA_ARGS__)

#define __UHC_CALL_LOGGER_ERROR__(controller, message, ...) \
  __STRUCT_CALL__(controller->controller_logger, error_c, (message), ## __VA_ARGS__)

#define __UHC_CALL_LOGGER_DEBUG__(controller, message, ...) \
  __STRUCT_CALL__(controller->controller_logger, debug_c, (message), ## __VA_ARGS__)

#define __UHC_CAST__(sub_controller) \
  __CAST__(__UHC__*, sub_controller)

#define __UHC_MEMORY__(sub_controller, name) \
  __MEM_SERVICE__(__UHC_CAST__(sub_controller)->mem_service, name)

#define __UHC_GET__(sub_controller, field) \
  __GET_FROM_SUPER__(__UHC_CAST__(sub_controller), field)

#define __UHC_SET__(sub_controller, field, cmd) \
  __SET_IN_SUPER__(__UHC_CAST__(sub_controller), field, cmd)

#define __UHC_ACQUIRE_LOCK__(sub_controller) \
  __STRUCT_CALL__(__UHC_CAST__(sub_controller)->controller_mutex, acquire_c)

#define __UHC_RELEASE_LOCK__(sub_controller) \
  __STRUCT_CALL__(__UHC_CAST__(sub_controller)->controller_mutex, release_c)

#define __UHC_CHECK_DEFAULT_VAL__ 0xFF

#define __UHC_DEFAULT_CHECK__(val) \
  val != __UHC_CHECK_DEFAULT_VAL__

#define __UHC_CHECK_CONDITION__(condition, ret) \
  if (condition) {         \
    return ret;               \
  }

#define __UHC_CHECK_ID_VENDOR(dev_id, desc_id, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.idVendor) \
    && dev_id.idVendor != desc_id.idVendor, ret)

#define __UHC_CHECK_ID_PRODUCT(dev_id, desc_id, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.idProduct) \
    && dev_id.idProduct != desc_id.idProduct, ret)

#define __UHC_CHECK_BCD_DEVICE(dev_id, desc_id, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.bcdDevice_high) \
    && dev_id.bcdDevice_high != USB_MAJOR_VERSION(desc_id.bcdDevice)   \
    && dev_id.bcdDevice_low != USB_MINOR_VERSION(desc_id.bcdDevice), ret)

#define __UHC_CHECK_DEV_CLASS(dev_id, desc_id, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.bDeviceClass) \
    && dev_id.bDeviceClass != desc_id.bDeviceClass, ret)

#define __UHC_CHECK_DEV_SUB_CLASS(dev_id, desc_id, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.bDeviceSubClass) \
    && dev_id.bDeviceSubClass != desc_id.bDeviceSubClass, ret)

#define __UHC_CHECK_DEV_PROTOCOL(dev_id, desc_id, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.bDeviceProtocol) \
    && dev_id.bDeviceProtocol != desc_id.bDeviceProtocol, ret)

#define __UHC_CHECK_ITF_CLASS(dev_id, itf_desc, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.bInterfaceClass) \
    && dev_id.bInterfaceClass != itf_desc.bInterfaceClass, ret)
#define __UHC_CHECK_ITF_SUB_CLASS(dev_id, itf_desc, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.bInterfaceSubClass) \
    && dev_id.bInterfaceSubClass != itf_desc.bInterfaceSubClass, ret)
#define __UHC_CHECK_ITF_PROTOCOL(dev_id, itf_desc, ret) \
  __UHC_CHECK_CONDITION__(__UHC_DEFAULT_CHECK__(dev_id.bInterfaceProtocol) \
    && dev_id.bInterfaceProtocol != itf_desc.bInterfaceProtocol, ret)

#define __UHC_DEFAULT_TABLE_CHECK__(name, field, table) \
  table[name].field != 0

#define __UHC_FOR_DEVICE_ID__(name, dev_id_table) \
  for(int name = 0;             \
    __UHC_DEFAULT_TABLE_CHECK__(name, idVendor, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, idProduct, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, bcdDevice_low, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, bcdDevice_high, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, bDeviceClass, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, bDeviceSubClass, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, bDeviceProtocol, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, bInterfaceClass, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, bInterfaceSubClass, dev_id_table) || \
    __UHC_DEFAULT_TABLE_CHECK__(name, bInterfaceProtocol, dev_id_table); \
    name++)

#define __DEFAULT_REMOVABLE_VALUE 0xFF

#define __DEFAULT_DEV__(speed, pn, controller, mem_service) \
  __NEW__(mem_service, UsbDev, sizeof(UsbDev), dev, new_usb_device, \
    new_usb_device, speed, pn, 0, __DEFAULT_REMOVABLE_VALUE, \
    pn, pn, controller, 0)

#define __SUPRESS_DEV__(speed, start_pn, level, removable, rootport, \
  device_num, controller, mem_service, name) \
  __NEW__(mem_service, UsbDev, sizeof(UsbDev), name, new_usb_device, \
    new_usb_device, speed, start_pn, level, removable, rootport, \
    device_num, controller, 1)

enum UsbControllerType {
  TYPE_UHCI = 0x01,
  TYPE_OHCI = 0x02,
  TYPE_EHCI = 0x03,
  TYPE_xHCI = 0x04
};

typedef enum UsbControllerType UsbControllerType;

struct UsbController {
  void (*new_usb_controller)(struct UsbController *usb_controller, SystemService_C* mem_service,
                             PciDevice_Struct* pci_device, const char *name);
  void (*poll)(struct UsbController *usb_controller);
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
  void (*iso)(struct UsbController* controller, Interface* interface, 
    unsigned int pipe, uint8_t priority, void* data, unsigned int len, uint16_t interval,
    callback_function callback);
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
  void (*iso_entry_point)(UsbDev* dev, Endpoint* endpoint, void* data, unsigned int len, uint8_t priority, uint16_t interval, callback_function callback);
  EventDispatcher* (*request_event_dispatcher)(struct UsbController* controller);
  Logger_C* (*init_controller_logger)(struct UsbController* controller);
  Register *(*look_up)(struct UsbController* controller, Register_Type r);
  Addr_Region* (*addr_address_region)(struct UsbController* controller, PciDevice_Struct* pci_device);
  Register** (*add_registers)(struct UsbController* controller, Addr_Region* addr_region);
  void (*dump_devices)(struct UsbController* controller);
  void (*dump_drivers)(struct UsbController* controller);
  void (*runnable_function)(struct UsbController* controller);
  void (*handler_function)(struct UsbController* controller);
  int (*remove_controller_linkage)(struct UsbController* controller, UsbDriver* driver);
  int (*remove_device_linkage)(struct UsbController* controller, UsbDriver* driver);

  const char *name;
  EventDispatcher* dispatcher;
  Addr_Region *addr_region;
  PciDevice_Struct *pci_device;
  SystemService_C *mem_service;
  SystemService_C *interrupt_service;
  SuperMap* interface_dev_map;
  SuperMap *register_look_up;
  Mutex_C* controller_mutex;
  Logger_C *controller_logger;
  Register** registers;
  uint8_t irq;
  list_head head_dev;
  list_head head_driver;
  list_element l_e; // service linkage
};

typedef struct UsbController UsbController;

void new_super_usb_controller(UsbController *controller, SystemService_C* m, 
                              PciDevice_Struct* pci_device, const char *name);

static inline size_t __REGISTER_READ(UsbController* controller,
                                     Register_Type r_t, 
                                     uint32_t* r_value){
  Register* reg = __STRUCT_CALL__(controller, look_up, r_t);
  return __STRUCT_CALL__(reg, read, r_value);
}

static inline size_t __REGISTER_WRITE(UsbController* controller, 
                                    Register_Type r_t, 
                                    uint32_t r_value){
  Register* reg = __STRUCT_CALL__(controller, look_up, r_t);
  return __STRUCT_CALL__(reg, write, &r_value);
}

static inline void __add_look_up_register(UsbController* controller,
                                          Register_Type* r_t,
                                          Register* reg) {
  __STRUCT_CALL__(controller->register_look_up, put_c, r_t, reg);
}

static inline void __add_look_up_registers(UsbController* controller, int range) {
  Register** registers = controller->registers;
  __FOR_RANGE__(i, int, 0, range) {
    Register_Type lvalue = registers[i]->type_of(registers[i]);
    __add_look_up_register(controller, &lvalue, registers[i]);
  }
}

extern const char *xHCI_name;
extern const char *EHCI_name;
extern const char *OHCI_name;
extern const char *UHCI_name;
extern const char *CONTROL_TRANSFER;
extern const char *BULK_TRANSFER;
extern const char *ISO_TRANSFER;
extern const char *INTERRUPT_TRANSFER;
extern const char *SETUP_TRANSACTION;
extern const char *DATA_IN_TRANSACTION;
extern const char *DATA_OUT_TRANSACTION;
extern const char *STATUS_TRANSACTION;

#endif