#include "HubDriver.h"
#include "../../dev/UsbDevice.h"
#include "../../include/UsbControllerInclude.h"
#include "../../include/UsbErrors.h"
#include "../../include/UsbInterface.h"
#include "../../interfaces/LoggerInterface.h"
#include "../../interfaces/ThreadInterface.h"
#include "../UsbDriver.h"

static int read_hub_status(HubDriver* driver, HubDev* dev, Interface* itf, uint8_t* data, unsigned int len);
static int read_hub_descriptor(HubDriver* driver, HubDev* dev, Interface* itf, uint8_t* data);
static int configure_hub(HubDriver* driver);
static void configure_callback(UsbDev* dev, Interface* interface, uint32_t status, void* data);
static int set_hub_feature(HubDriver* driver, HubDev* dev, Interface* itf, uint16_t port, uint16_t feature);
static int clear_hub_feature(HubDriver* driver, HubDev* dev, Interface* itf, uint16_t port, uint16_t feature);
static void callback_hub(UsbDev* dev, Interface* interface, uint32_t status, void* data);
static int16_t probe_hub(UsbDev* dev, Interface* interface);
static void disconnect_hub(UsbDev* dev, Interface* interface);
static void dump_port_status_change(HubDriver* driver, uint16_t* port_status_change_field);
static void dump_port_status(HubDriver* driver, uint16_t* port_status_field);
static uint8_t is_device_removable(HubDriver* driver, HubDev* hub_dev, uint8_t downstream_port);
static HubDev* get_free_hub_dev(HubDriver* driver);
static HubDev* match_hub_dev(HubDriver* driver, UsbDev* dev);
static void free_hub_dev(HubDriver* driver, HubDev* hub_dev);
static void get_hub_descriptor(HubDriver* driver, UsbDev* dev, UsbDeviceRequest* device_req,
  void* data, unsigned int len, callback_function callback);

static HubDriver *internal_hub_driver = 0;

// if we use plug & play this callback would listen to the interrupt transfer
// which checks the status of the downstream ports
static void callback_hub(UsbDev *dev, Interface* interface, uint32_t status, void *data) {}

static int16_t probe_hub(UsbDev *dev, Interface *interface) {
  Endpoint **endpoints = interface->active_interface->endpoints;
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;
  int e = interface_desc.bNumEndpoints;
  MemoryService_C *mem_service = __DEV_MEMORY(dev);

  __REQUEST_OR_LEAVE__(HubDev, internal_hub_driver, get_free_hub_dev, hub_dev);
  // select only 1 endpoint
  __FOR_RANGE__(i, int, 0, e) {
    __DRIVER_DIRECTION_IN__(__STRUCT_CALL__(internal_hub_driver, free_hub_dev, hub_dev), 
      dev, endpoints[i]);
    __DRIVER_TYPE_INTERRUPT__(__STRUCT_CALL__(internal_hub_driver, free_hub_dev, hub_dev),
      dev, endpoints[i]);
    if (!hub_dev->endpoint_addr) {
      uint8_t *key_board_buffer =
          (uint8_t *)mem_service->mapIO(mem_service, sizeof(uint8_t), 1);

      hub_dev->endpoint_addr = __STRUCT_CALL__(dev, __endpoint_number, endpoints[i]);
      hub_dev->usb_dev = dev;
      hub_dev->buffer = key_board_buffer;
      hub_dev->buffer_size = 0;
      hub_dev->priority = PRIORITY_8;
      hub_dev->interface = interface;
      hub_dev->interval = endpoints[i]->endpoint_desc.bInterval;
      return __RET_S__;
    }
  }
  return __RET_E__;
}

static void disconnect_hub(UsbDev *dev, Interface *interface) {}

void new_hub_driver(HubDriver *driver, char *name, UsbDevice_ID *entry) {
  for (int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++) {
    driver->hub_map[i] = 0;
    driver->dev[i].transfer_success = 0;
    driver->dev[i].x_powered = 0;
    driver->dev[i].x_wakeup = 0;
    driver->dev[i].usb_dev = 0;
    driver->dev[i].endpoint_addr = 0;
    driver->dev[i].buffer = 0;
    driver->dev[i].buffer_size = 0;
    driver->dev[i].priority = 0;
    driver->dev[i].interface = 0;
    driver->dev[i].interval = 0;
    driver->dev[i].callback = &callback_hub;
    driver->dev[i].driver = (UsbDriver *)driver;
  }

  internal_hub_driver = driver;
  __INIT_HUB_DRIVER__(driver, name, entry);
}

static HubDev *get_free_hub_dev(HubDriver *driver) {
  __GET_FREE_DEV__(HubDev, driver->dev, driver->hub_map);
}

static HubDev *match_hub_dev(HubDriver *driver, UsbDev *dev) {
  __MATCH_DEV__(HubDev, driver->dev, usb_dev, dev);
}

static void free_hub_dev(HubDriver *driver, HubDev *hub_dev) {
  __FREE_DEV__(hub_dev, driver->dev, driver->hub_map);
}

static void configure_callback(UsbDev *dev, Interface* interface, uint32_t status, void *data) {
  HubDev *hub_dev =
      internal_hub_driver->match_hub_dev(internal_hub_driver, dev);

  if (hub_dev == (void *)0)
    return;

  if (status == S_TRANSFER) {
    hub_dev->transfer_success = 1;
  } else
    hub_dev->transfer_success = 0;
}

// stacking hub's currently not supported ! -> maybe implemented in future
static int configure_hub(HubDriver *driver) {
  for (int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++) {
    if (driver->hub_map[i] == 0)
      continue;
    UsbDev *dev = driver->dev[i].usb_dev;
    Interface *itf = driver->dev[i].interface;

    HubDev *hub_dev = driver->dev + i;

    MemoryService_C *m = __DEV_MEMORY(dev);
    __MAP_IO_KERNEL_S__(m, uint8_t, data);

    if (driver->read_hub_descriptor(driver, hub_dev, itf, data) == -1)
      return -1;

    if (driver->read_hub_status(driver, hub_dev, itf, data, 0x02) == -1)
      return -1;
    driver->dev[i].x_powered = *data & SELF_POWERED;
    driver->dev[i].x_wakeup = *data & REMOTE_WAKEUP;

    uint8_t wait_time = driver->dev[i].hub_desc.potpgt;
    uint8_t multiplicator_wait_time_ms = 2;

    uint8_t num_ports = driver->dev[i].hub_desc.num_ports;

    __DRIVER_LOGGER_INFO_CALL__(driver,
      "Detected %d downstream ports , minor %d", num_ports, i);

    dev->add_downstream(dev, num_ports);

    uint16_t port_status_field;
    //uint16_t port_change_status_field;
    uint8_t device_attached_mask = 0x01;
    uint8_t start_device_num = 0x03 + ((dev->dev_num-1) * 8);

    for (uint8_t start_port = 0x01; start_port <= num_ports; start_port++) {
      if (driver->set_hub_feature(driver, hub_dev, itf, start_port,
                                  PORT_POWER) == -1)
        return -1;
      mdelay(wait_time * multiplicator_wait_time_ms);

      if (driver->clear_hub_feature(driver, hub_dev, itf, start_port,
                                    C_PORT_OVER_CURRENT) == -1)
        return -1;

      if (driver->clear_hub_feature(driver, hub_dev, itf, start_port,
                                    C_PORT_CONNECTION) == -1)
        return -1;

      if (driver->read_hub_status(driver, hub_dev, itf, data, 0x04) == -1)
        return -1;

      port_status_field = *((uint16_t *)data);
      //port_change_status_field = *((uint16_t *)(data + 2));

      if (port_status_field & device_attached_mask) { // device attached
        if (driver->set_hub_feature(driver, hub_dev, itf, start_port,
                                    PORT_RESET) == -1)
          return -1;

        if (driver->clear_hub_feature(driver, hub_dev, itf, start_port,
                                      C_PORT_RESET) == -1)
          return -1;

        uint8_t speed =
            ((port_status_field & 0x200) >> 9) == 1 ? LOW_SPEED : FULL_SPEED;
        uint8_t removable =
            driver->is_device_removable(driver, hub_dev, start_port);
        uint8_t level = dev->level + 1;

        __SUPRESS_DEV__(speed, start_port, level, removable, dev->rootport, start_device_num,
          dev->controller, m, new_dev);

        if (new_dev->error_while_transfering) {
          new_dev->delete_usb_dev(new_dev);
        } else {
          dev->add_downstream_device(dev, new_dev);
          start_device_num++;
          __DRIVER_LOGGER_INFO_CALL__(driver,
          "Succesful configured Usb-Device on port : %d , "
          "Device : %s, %s",
          start_port, new_dev->manufacturer, new_dev->product);
        }
      }
    }
    m->unmap(m, data, 1);
  }

  return 1;
}

// if bit is set -> non removable
static uint8_t is_device_removable(HubDriver *driver, HubDev *hub_dev,
                            uint8_t downstream_port) {
  uint8_t *removable_x = hub_dev->hub_desc.x;
  uint8_t mask = 0x01;

  if (downstream_port < 8) {
    return ((removable_x[0] >> downstream_port) & mask) ^ mask;
  }
  uint8_t byte_num = downstream_port / sizeof(uint8_t);
  uint8_t shift = downstream_port % sizeof(uint8_t);
  return (removable_x[byte_num] >> shift) ^ mask;
}

static void dump_port_status(HubDriver *driver, uint16_t *port_status_field) {
  uint8_t mask = 0x01;
  uint8_t port_indicator = (*port_status_field >> PORT_INDICATOR_STATUS) & mask;
  uint8_t port_test = (*port_status_field >> PORT_TEST_STATUS) & mask;
  uint8_t port_high_speed =
      (*port_status_field >> PORT_DEVICE_ATTACHED_HIGH_SPEED_STATUS) & mask;
  uint8_t port_low_speed =
      (*port_status_field >> PORT_DEVICE_ATTACHED_LOW_SPEED_STATUS) & mask;
  uint8_t port_power = (*port_status_field >> PORT_POWER_STATUS) & mask;
  uint8_t port_reset = (*port_status_field >> PORT_RESET_STATUS) & mask;
  uint8_t port_over_current =
      (*port_status_field >> PORT_OVER_CURRENT_STATUS) & mask;
  uint8_t port_suspend = (*port_status_field >> PORT_SUSPEND_STATUS) & mask;
  uint8_t port_enable = (*port_status_field >> PORT_ENABLE_STATUS) & mask;
  uint8_t port_connect = *port_status_field & mask;

  __DRIVER_LOGGER_DEBUG_CALL__(driver,
      "PORT_STATUS :\n\n\tport indicator : %u\n\tport test : %u\n\tport high "
      "speed attached : %u\n"
      "\tport low speed attached : %u\n\tport power : %u\n\tport reset : "
      "%u\n\tport over current : %u\n"
      "\tport suspend : %u\n\tport enable : %u\n\tport connect : %u\n",
      port_indicator, port_test, port_high_speed, port_low_speed, port_power,
      port_reset, port_over_current, port_suspend, port_enable, port_connect);
}

static void dump_port_status_change(HubDriver *driver,
                             uint16_t *port_status_change_field) {
  uint8_t mask = 0x01;
  uint8_t reset_change =
      (*port_status_change_field >> PORT_RESET_CHANGE_STATUS) & mask;
  uint8_t over_current_change =
      (*port_status_change_field >> PORT_OVER_CURRENT_CHANGE_STATUS) & mask;
  uint8_t suspend_change =
      (*port_status_change_field >> PORT_SUSPEND_CHANGE_STATUS) & mask;
  uint8_t enable_change =
      (*port_status_change_field >> PORT_ENABLE_CHANGE_STATUS) & mask;
  uint8_t connect_change = *port_status_change_field & mask;

  __DRIVER_LOGGER_DEBUG_CALL__(driver,
      "PORT CHANGE STATUS :\n\n\treset change : %u\n\tover current change : "
      "%u\n\tsuspend change : %u\n"
      "\tenable change : %u\n\tconnect change : %u\n",
      reset_change, over_current_change, suspend_change, enable_change,
      connect_change);
}

static int read_hub_status(HubDriver *driver, HubDev *hub_dev, Interface *itf,
                    uint8_t *data, unsigned int len) {
  if (hub_dev->usb_dev->get_req_status(hub_dev->usb_dev, itf, data, len,
                                       &configure_callback) == -1)
    return -1;

  if (!hub_dev->transfer_success)
    return -1;
  return 1;
}

static void get_hub_descriptor(HubDriver* driver, UsbDev* dev, UsbDeviceRequest* device_req,
  void* data, unsigned int len, callback_function callback){
  __STRUCT_CALL__(dev, request_build, device_req,
    DEVICE_TO_HOST | TYPE_REQUEST_CLASS | RECIPIENT_DEVICE,
    GET_DESCRIPTOR, 0x2900, 0, 0, 0, len);
  __STRUCT_CALL__(dev, request, device_req, data, PRIORITY_8, 0, 0, callback,
    CONTROL_INITIAL_STATE);
}

static int read_hub_descriptor(HubDriver *driver, HubDev *hub_dev, Interface *itf,
                        uint8_t *data) {
  UsbDev* dev = hub_dev->usb_dev;
  UsbDeviceRequest* device_req = __STRUCT_CALL__(dev, get_free_device_request);
  __IF_RET_NEG__(__IS_NULL__(device_req));
  get_hub_descriptor(driver, dev, device_req, data, 2, &configure_callback);

  __IF_RET_NEG__(!hub_dev->transfer_success);

  HubDescriptor *hub_desc = (HubDescriptor *)data;
  uint8_t hub_desc_len = hub_desc->len;
  get_hub_descriptor(driver, dev, device_req, data, hub_desc_len, &configure_callback);
  
  __IF_RET_NEG__(!hub_dev->transfer_success);
  hub_desc = (HubDescriptor *)data;
  hub_dev->hub_desc = *hub_desc;

  return 1;
}

static int set_hub_feature(HubDriver *driver, HubDev *hub_dev, Interface *itf,
                    uint16_t port, uint16_t feature) {
  UsbDev* usb_dev = hub_dev->usb_dev;
  UsbDeviceRequest* device_req = usb_dev->get_free_device_request(hub_dev->usb_dev);
  __IF_RET_NEG__(__IS_NULL__(device_req));
  usb_dev->request_build(usb_dev, device_req,
                  HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_OTHER,
                  SET_FEATURE, 0, feature, 0, port, 0);
  usb_dev->request(usb_dev, device_req, 0, PRIORITY_8, 0, 0, configure_callback, CONTROL_INITIAL_STATE);

  if (!hub_dev->transfer_success)
    return -1;

  return 1;
}

static int clear_hub_feature(HubDriver *driver, HubDev *hub_dev, Interface *itf,
                      uint16_t port, uint16_t feature) {
  UsbDev* usb_dev = hub_dev->usb_dev;
  UsbDeviceRequest* device_req = usb_dev->get_free_device_request(hub_dev->usb_dev);
  __IF_RET_NEG__(__IS_NULL__(device_req));
  usb_dev->request_build(usb_dev, device_req,
                     HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_OTHER,
                     CLEAR_FEATURE, 0, feature, 0, port, 0);
  usb_dev->request(usb_dev, device_req, 0, PRIORITY_8, 0, 0, configure_callback, CONTROL_INITIAL_STATE);
  
  if (!hub_dev->transfer_success)
    return -1;

  return 1;
}