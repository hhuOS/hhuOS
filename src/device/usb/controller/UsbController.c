#include "UsbController.h"
#include "../events/EventDispatcher.h"
#include "../include/UsbControllerInclude.h"
#include "../interfaces/MapInterface.h"
#include "../interfaces/SystemInterface.h"
#include "../include/UsbErrors.h"

const char *xHCI_name = "xHCI";
const char *EHCI_name = "EHCI";
const char *OHCI_name = "OHCI";
const char *UHCI_name = "UHCI";

const char *CONTROL_TRANSFER = "Control";
const char *BULK_TRANSFER = "Bulk";
const char *ISO_TRANSFER = "Iso";
const char *INTERRUPT_TRANSFER = "Interrupt";

const char *SETUP_TRANSACTION = "Setup";
const char *DATA_IN_TRANSACTION = "Data_IN";
const char *DATA_OUT_TRANSACTION = "Data_OUT";
const char *STATUS_TRANSACTION = "Status";

static EventDispatcher *request_event_dispatcher(UsbController* controller);
static Logger_C* init_logger(UsbController* controller);
static int insert_listener(UsbController *controller, EventListener *listener);
static int delete_listener(UsbController *controller, int id);
static int insert_callback(UsbController *controller, uint16_t reg_type,
                    event_callback callback);
static int delete_callback(UsbController *controller, uint16_t reg_type,
                    event_callback callback);
static int supported_event_listener_type(UsbController *controller, 
                    uint16_t event_listener_type);
static void add_device(UsbController *controller, UsbDev *dev);
static int contain_device(UsbController *controller, UsbDev *dev);
static Register *look_for_reg(UsbController* controller, Register_Type r);
static int register_driver(UsbController *controller, UsbDriver *driver);
static void link_device_to_driver(UsbController *controller, UsbDev *dev,
                                UsbDriver *driver);
static void link_driver_to_controller(UsbController *controller,
                                    UsbDriver *driver);
static void link_driver_to_interface(UsbController *controller, UsbDriver *driver,
                                   Interface *interface);
static int deregister_driver(UsbController *controller, UsbDriver *driver);
static void dump_devices(UsbController* controller);
static void dump_drivers(UsbController* controller);
static int remove_controller_linkage(UsbController* controller, UsbDriver* driver);
static int remove_device_linkage(UsbController* controller, UsbDriver* driver);

static void inline __build_uhc(UsbController* controller, const char* name){
  controller->name = name;
  controller->l_e.l_e = 0;
  controller->head_dev.l_e = 0;
  controller->head_driver.l_e = 0;

  __INIT_CONTROLLER__(controller, name);
}

static inline int __match_device(UsbDevice_ID device_id, 
                               DeviceDescriptor dev_desc) {
  int __ret_v = __RET_N__;
  __UHC_CHECK_ID_VENDOR(device_id, dev_desc, __ret_v) 
  __UHC_CHECK_ID_PRODUCT(device_id, dev_desc, __ret_v)    
  __UHC_CHECK_BCD_DEVICE(device_id, dev_desc, __ret_v)    
  __UHC_CHECK_DEV_CLASS(device_id, dev_desc, __ret_v)     
  __UHC_CHECK_DEV_SUB_CLASS(device_id, dev_desc, __ret_v) 
  __UHC_CHECK_DEV_PROTOCOL(device_id, dev_desc, __ret_v)                 
  return __RET_S__;
}

static inline int __match_interface(UsbDevice_ID device_id,
                                    InterfaceDescriptor itf_desc) {
  int __ret_v = __RET_N__;
  __UHC_CHECK_ITF_CLASS(device_id, itf_desc, __ret_v)
  __UHC_CHECK_ITF_SUB_CLASS(device_id, itf_desc, __ret_v)
  __UHC_CHECK_ITF_PROTOCOL(device_id, itf_desc, __ret_v)
  return __RET_S__;
}

static inline int __probe_routine(UsbController* controller, UsbDev* dev,
                                   Interface* interface, UsbDriver* driver) {
  if (driver->probe(dev, interface) < 0) {
    dev->usb_dev_free_interface(dev, interface);
    return __RET_N__;
  }
  __STRUCT_CALL__(controller, link_device_to_driver, dev, driver);
  __STRUCT_CALL__(controller, link_driver_to_interface, driver, interface);
  __STRUCT_CALL__(controller->interface_dev_map, put_c, interface, dev);
  driver->dispatcher = controller->dispatcher; 

  return __RET_S__;        
}

void new_super_usb_controller(UsbController *controller, SystemService_C *mem_service, 
                              PciDevice_Struct* pci_device, const char *name) {
  __build_uhc(controller, name);
  __MEM_SERVICE__(mem_service, m);
  controller->mem_service = mem_service;
  controller->pci_device = pci_device;
  __NEW__(m, Mutex_C, sizeof(Mutex_C), mutex, new_mutex, new_mutex);
  __NEW__(m, Interface_Device_Map, sizeof(Interface_Device_Map), interface_device_map,
    new_map, newInterface_Device_Map, "Map<Interface*,UsbDev*>");
  __NEW__(m, Register_Map, sizeof(Register_Map), register_map, new_map, newRegisterMap, 
    "Map<RegisterType*,Register*>");
  __NEW__(m, InterruptService_C, sizeof(InterruptService_C), interrupt_service, 
    new_interrupt_service, new_interrupt_service);

  controller->controller_mutex = mutex;
  controller->interrupt_service = (SystemService_C*)interrupt_service;
  controller->interface_dev_map = (SuperMap*)interface_device_map;
  controller->register_look_up = (SuperMap*)register_map;
  controller->addr_region = controller->addr_address_region(controller, pci_device);
  controller->registers = controller->add_registers(controller, controller->addr_region);
  controller->dispatcher = controller->request_event_dispatcher(controller);
  controller->controller_logger = controller->init_controller_logger(controller);
  interrupt_service->add_interrupt_routine(interrupt_service, controller->irq, controller);
}

static Logger_C *init_logger(UsbController *controller) {
  __MEM_SERVICE__(controller->mem_service, m);
  uint8_t type = USB_CONTROLLER_LOGGER_TYPE, level = 0;

#if defined(DEBUG_ON) || defined(TD_DEBUG_ON) || defined(QH_DEBUG_ON) ||       \
    defined(TRANSFER_DEBUG_ON) || defined(DEVICE_DEBUG_ON) ||                  \
    defined(SKELETON_DEBUG_ON) || defined(REGISTER_DEBUG_ON) ||                \
    defined(STATUS_DEBUG_ON)
  level = LOGGER_LEVEL_DEBUG;
#else
  level = LOGGER_LEVEL_INFO;
#endif
  __NEW__(m, Logger_C, sizeof(Logger_C), logger, new_logger, new_logger,
    type, level);

  return logger;
}

static EventDispatcher *request_event_dispatcher(UsbController *controller) {
  __MEM_SERVICE__(controller->mem_service, m);
  __NEW__(m, EventDispatcher, sizeof(EventDispatcher), dispatcher, 
          new_event_dispatcher, new_event_dispatcher);
  __STRUCT_CALL__(dispatcher, init_event_dispatcher_map);

  return dispatcher;
}

static Register *look_for_reg(UsbController* controller, Register_Type r) {
  return (Register*)__STRUCT_CALL__(controller->register_look_up, get_c, &r);
}

static void add_device(UsbController *controller, UsbDev *dev) {
  controller->controller_mutex->acquire_c(controller->controller_mutex);
  if (controller->head_dev.l_e == (void *)0) {
    controller->head_dev.l_e = &dev->l_e;
  } 
  else {
    list_element *l_e = controller->head_dev.l_e;
    __LIST_TRAVERSE__(l_e);
    l_e->l_e = &dev->l_e;
  }

  controller->controller_mutex->release_c(controller->controller_mutex);
}

static int contain_device(UsbController *controller, UsbDev *dev) {
  controller->controller_mutex->acquire_c(controller->controller_mutex);
  list_element *l_e = controller->head_dev.l_e;
  __LIST_FOR_EACH__(l_e) {
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

// -1 -> fault
// >= 0 -> id of listener in dispatcher
static int insert_listener(UsbController *controller, EventListener *listener) {
  if (listener == (void *)0)
    return -1;

  return controller->dispatcher->register_event_listener(controller->dispatcher,
                                                         listener);
}

static int delete_listener(UsbController *controller, int id) {
  if (id < 0)
    return -1;

  return controller->dispatcher->deregister_event_listener(
      controller->dispatcher, id);
}

static int insert_callback(UsbController *controller, uint16_t reg_type,
                    event_callback callback) {
  if (callback == (void *)0)
    return -1;

  if (!controller->supported_event_listener_type(controller, reg_type))
    return -1;

  return controller->dispatcher->reg_callback(controller->dispatcher, callback,
                                              reg_type);
}

static int supported_event_listener_type(UsbController *controller,
                                  uint16_t event_listener_type) {
  int supported = 0;

  if (event_listener_type == MOUSE_LISTENER) {
    supported = 1;
  } else if (event_listener_type == KEY_BOARD_LISTENER) {
    supported = 1;
  }

  return supported;
}

static int delete_callback(UsbController *controller, uint16_t reg_type,
                    event_callback callback) {
  if (reg_type < 0)
    return -1;
  if (callback == (void *)0)
    return -1;

  if (!controller->supported_event_listener_type(controller, reg_type))
    return -1;

  return controller->dispatcher->dereg_callback(controller->dispatcher,
                                                callback, reg_type);
}

static int register_driver(UsbController *controller, UsbDriver *driver) {
  list_element *first_dev = controller->head_dev.l_e;
  UsbDevice_ID *device_id_table = driver->entry;
  int driver_device_match_count = 0;

  if (driver->probe == (void *)0) {
    return -1;
  };
  __STRUCT_CALL__(controller, link_driver_to_controller, driver);

  __UHC_FOR_DEVICE_ID__(i, device_id_table){
    UsbDevice_ID device_id = device_id_table[i];
    __FOR_RANGE_COND__(copy_devs, list_element*, first_dev, copy_devs != (void *)0, 
      copy_devs = copy_devs->l_e){
      UsbDev *dev = (UsbDev *)container_of(copy_devs, UsbDev, l_e);
      DeviceDescriptor device_desc = dev->device_desc;
      // 0xFF is default value -> if change check
      __IF_CONTINUE__(!__match_device(device_id, device_desc))

      Configuration *config = dev->active_config;
      int interface_num = config->config_desc.bNumInterfaces;

      __FOR_RANGE__(k, int, 0, interface_num) {
        Interface *interface = config->interfaces[k];
        Alternate_Interface *alt_interface = interface->active_interface;
        __IF_CONTINUE__(!__match_interface(device_id, alt_interface->alternate_interface_desc));

        int status = dev->usb_dev_interface_lock(dev, interface, driver);

        __IF_CONTINUE__((status == E_INTERFACE_IN_USE) || (status == E_INTERFACE_INV))

        driver_device_match_count += __probe_routine(controller, dev, interface, driver);
      }
    }
  }

  return __IF_EXT__(driver_device_match_count == 0, -1, driver_device_match_count);
}

static void link_device_to_driver(UsbController *controller, UsbDev *dev,
                                UsbDriver *driver) {
  list_element *l_e = driver->head.l_e;

  __USB_LOCK__(controller);

  if (l_e == (void *)0) {
    driver->head.l_e = &dev->l_e_driver;
    __USB_RELEASE__(controller);
    return;
  }
  __LIST_TRAVERSE__(l_e);

  __USB_RELEASE__(controller);

  l_e->l_e = &dev->l_e_driver;
}

static void link_driver_to_controller(UsbController *controller,
                                    UsbDriver *driver) {
  list_element *l_e = controller->head_driver.l_e;

  __USB_LOCK__(controller);

  if (l_e == (void *)0) {
    controller->head_driver.l_e = &driver->l_e;
    __USB_RELEASE__(controller);
    return;
  }
  __LIST_TRAVERSE__(l_e);
  __USB_RELEASE__(controller);

  l_e->l_e = &driver->l_e;
}

static void link_driver_to_interface(UsbController *controller, UsbDriver *driver,
                                   Interface *interface) {
  interface->driver = (void *)driver;
}

static int deregister_driver(UsbController *controller, UsbDriver *driver) {
  return __STRUCT_CALL__(controller, remove_controller_linkage, driver) |
    __STRUCT_CALL__(controller, remove_device_linkage, driver);
}

static int remove_controller_linkage(UsbController* controller, UsbDriver* driver){
  list_element *prev;
  list_element *l_e = controller->head_driver.l_e;

  UsbDriver *d = (UsbDriver *)container_of(l_e, UsbDriver, l_e);

  if (l_e == (void *)0)
    return DRIVER_NOT_FOUND;

  __USB_LOCK__(controller);

  if (d == driver) {
    controller->head_driver.l_e = controller->head_driver.l_e->l_e;
    return DRIVER_REMOVED;
  } 
  prev = l_e;
  l_e = l_e->l_e;
  __LIST_FOR_EACH__(l_e){
    d = (UsbDriver *)container_of(l_e, UsbDriver, l_e);
    if (driver == d) {
      prev->l_e = l_e->l_e;
      return DRIVER_REMOVED;
    }
    l_e = l_e->l_e;
  }
  
  __USB_RELEASE__(controller);

  return DRIVER_NOT_FOUND;
}

static int remove_device_linkage(UsbController* controller, UsbDriver* driver){
  list_element *l_dev = controller->head_dev.l_e;

  __USB_LOCK__(controller);

  __LIST_FOR_EACH__(l_dev) {
    UsbDev *dev = (UsbDev *)container_of(l_dev, UsbDev, l_e);
    int interface_num = dev->active_config->config_desc.bNumInterfaces;
    Interface **interfaces = dev->active_config->interfaces;
    for (int i = 0; i < interface_num; i++) {
      if (((UsbDriver *)interfaces[i]->driver) == driver) {
        dev->usb_dev_free_interface(dev, interfaces[i]);
        __USB_RELEASE__(controller);
        return DRIVER_LINKED;
      }
    }
    l_dev = l_dev->l_e;
  }

  __USB_RELEASE__(controller);

  return DRIVER_NOT_LINKED;
}

static void dump_drivers(UsbController* controller) {
  list_element *l_e = controller->head_dev.l_e;
  Logger_C *logger = controller->controller_logger; 
  char *message = "Controller = %s : %x\n";
  char *driver_message = "\tDriver : %s\n";
  char *device_message = "\t\tDevice : %s\n";

  __STRUCT_CALL__(logger, debug_c, message, controller->name, 
    __PTR_TYPE__(uint32_t, controller));
  while (l_e != (void *)0) {
    UsbDriver *usb_driver = (UsbDriver *)container_of(l_e, UsbDriver, l_e);
    __STRUCT_CALL__(logger, debug_c, driver_message, usb_driver->name);
    list_element *l = usb_driver->head.l_e;
    while (l != (void *)0) {
      UsbDev *dev = (UsbDev *)container_of(l, UsbDev, l_e_driver);
      __STRUCT_CALL__(logger, debug_c, device_message, dev->device_desc.idProduct);
      l = l->l_e;
    }
    l_e = l_e->l_e;
  }
}

static void dump_devices(UsbController* controller) {
  list_element *l_e = controller->head_dev.l_e;
  Logger_C *logger = controller->controller_logger; 
  char *message = "Controller = %s : %x\n";
  char *device_message = "\tDevice : %s\n";
  char *interface_message = "\t\t%d.%d : %s";

  __STRUCT_CALL__(logger, debug_c, message, controller->name, 
    __PTR_TYPE__(uint32_t, controller));

  while (l_e != (void *)0) {
    UsbDev *usb_dev = (UsbDev *)container_of(l_e, UsbDev, l_e);
    __STRUCT_CALL__(logger, debug_c, device_message, usb_dev->device_desc.idProduct);
    int config_value = usb_dev->active_config->config_desc.bConfigurationValue;
    int interface_num = usb_dev->active_config->config_desc.bNumInterfaces;
    Interface **interfaces = usb_dev->active_config->interfaces;
    for (int i = 0; i < interface_num; i++) {
      int interface_number =
          interfaces[i]
              ->active_interface->alternate_interface_desc.bInterfaceNumber;
      UsbDriver *used_driver = (UsbDriver *)interfaces[i]->driver;
      __STRUCT_CALL__(logger, debug_c, interface_message, config_value, 
        interface_number, used_driver->name);
    }
    l_e = l_e->l_e;
  }
}