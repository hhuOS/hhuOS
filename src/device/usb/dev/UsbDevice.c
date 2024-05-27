#include "UsbDevice.h"
#include "../controller/UsbController.h"
#include "../controller/UsbControllerFlags.h"
#include "../driver/UsbDriver.h"
#include "../driver/hub/HubDriver.h"
#include "../events/listeners/EventListener.h"
#include "../events/listeners/hid/KeyBoardListener.h"
#include "../events/listeners/hid/MouseListener.h"
#include "../include/UsbControllerInclude.h"
#include "../include/UsbErrors.h"
#include "../include/UsbGeneral.h"
#include "../interfaces/SystemInterface.h"
#include "../utility/Utils.h"
#include "data/UsbDev_Data.h"
#include "requests/UsbDescriptors.h"
#include "requests/UsbRequests.h"
#include "stdint.h"

static  void add_downstream_device(UsbDev* dev, UsbDev* downstream_dev);
static  void add_downstream(UsbDev* dev, uint8_t downstream_ports);
static void delete_usb_dev(UsbDev* dev);
static void free_usb_dev_strings(UsbDev* dev);
static void free_usb_dev_configs(UsbDev* dev);
static void free_usb_dev_interfaces(UsbDev* dev, Interface** interfaces, int num_interfaces);
static void free_usb_dev_endpoints(UsbDev* dev, Endpoint** endpoints, int num_endpoints);
 // pipe = endpoint information to create the pipe
static void usb_dev_control(UsbDev *dev, Interface *interface, unsigned int pipe,
                      uint8_t priority, void *data, uint8_t *setup,
                      callback_function callback, uint8_t flags);
static void usb_dev_interrupt(UsbDev *dev, Interface *interface, unsigned int pipe,
                        uint8_t priority, void *data, unsigned int len,
                        uint16_t interval, callback_function callback);
static void usb_dev_bulk(struct UsbDev *dev, Interface *interface, unsigned int pipe,
                   uint8_t priority, void *data, unsigned int len,
                   callback_function callback, uint8_t flags);
static void usb_dev_iso(UsbDev* dev, Interface* interface, unsigned int pipe,
  uint8_t priority, void* data, unsigned int len, uint16_t interval, 
  callback_function callback);
static int usb_dev_interface_lock(UsbDev *dev, Interface *interface, void* driver);
static void usb_dev_free_interface(UsbDev *dev, Interface *interface);
static void request_callback(struct UsbDev *dev, uint32_t status, void *data);
static void request_build(struct UsbDev *dev, UsbDeviceRequest *req, int8_t rq_type,
                    int8_t rq, int16_t value_high, int16_t value_low,
                    int16_t shift, int16_t index, int16_t len);
static void request(struct UsbDev *dev, struct UsbDeviceRequest *device_request,
              void *data, uint8_t priority, Endpoint *endpoint,
              callback_function callback, uint8_t flags);
static int8_t support_bulk(struct UsbDev *dev, Interface *interface);
static int8_t support_isochronous(struct UsbDev *dev, Interface *interface);
static int8_t support_control(struct UsbDev *dev, Interface *interface);
static int8_t support_interrupt(struct UsbDev *dev, Interface *interface);
static int8_t support_transfer_type(struct UsbDev* dev, Interface* interface, uint8_t transfer_type);
static int is_pipe_buildable(UsbDev* dev, Endpoint *endpoint, unsigned int pipe);
static int contain_interface(UsbDev *dev, Interface *interface);
static int request_switch_configuration(struct UsbDev *dev, int configuration,
                                  callback_function callback);
static int request_switch_alternate_setting(struct UsbDev *dev, Interface *interface,
                                      int setting);
static void dump_device_desc(struct UsbDev *dev);
static void dump_configuration(struct UsbDev *dev);
static void dump_interface(struct UsbDev *dev, Interface *interface);
static void dump_endpoints(struct UsbDev *dev, Interface *interface);
static void dump_endpoint(struct UsbDev *dev, Endpoint *endpoint);
static char *build_string(UsbDev *dev, int len, uint8_t *string_buffer);
static void process_lang_ids(UsbDev *dev, uint8_t *string_buffer, int s_len);
static void dump_device(struct UsbDev *dev);
static int get_max_logic_unit_numbers(UsbDev* dev, Interface* interface, uint8_t* data, callback_function callback);
static int reset_bulk_only(UsbDev* dev, Interface* interface, callback_function callback);
static UsbDeviceRequest *get_free_device_request(UsbDev *dev);
static void free_device_request(UsbDev *dev, UsbDeviceRequest *device_request);
static int set_report(UsbDev *dev, Interface *interface, uint8_t type, void *data,
                unsigned int len, callback_function callback);
static int set_protocol(UsbDev *dev, Interface *interface, uint16_t protocol_value,
                  callback_function callback);
static int set_idle(UsbDev *dev, Interface *interface);
static int process_device_descriptor(UsbDev *dev, DeviceDescriptor *device_descriptor,
                               unsigned int len, uint8_t supress_flag);
static int process_configuration_descriptor(UsbDev *dev,
                                      ConfigurationDescriptor *config_descriptor,
                                      uint16_t configuration_index,
                                      unsigned int len);
static int process_string_descriptor(UsbDev *dev, uint8_t *string_buffer,
                                 uint16_t index, uint16_t lang_id, char *s,
                                 unsigned int len);
static int set_address(UsbDev *dev, uint8_t address);
static int set_configuration(UsbDev *dev, uint8_t configuration);
static int process_whole_configuration(UsbDev *dev, uint8_t *configuration_buffer,
                                 uint16_t configuration_index, unsigned int len);
static int handle_interface(UsbDev* dev, Configuration* configuration, uint8_t* string_buffer, 
                       uint8_t* start, uint8_t* end, uint8_t num_interfaces);
static int handle_configuration(UsbDev* dev, uint8_t* string_buffer, uint8_t* config_buffer, 
                 ConfigurationDescriptor* config_descriptor, uint8_t num_configurations);
static int handle_lang(UsbDev* dev, uint8_t* string_buffer);
static int handle_dev(UsbDev* dev, uint8_t* string_buffer, DeviceDescriptor* device_descriptor);
static int get_descriptor(UsbDev* dev, Interface* interface, uint8_t* data, unsigned int len, callback_function callback);
static int get_req_status(UsbDev* dev, Interface* interface, uint8_t* data, unsigned int len, callback_function callback);
static int clear_feature(UsbDev* dev, Interface* interface, uint16_t feature_value, uint16_t port,
                        callback_function callback);
static int set_feature(UsbDev* dev, Interface* interface, uint16_t feature_value, uint16_t port, 
                       callback_function callback);
static char* string_build_routine(struct UsbDev* dev, unsigned int string_index,
    uint8_t* string_buffer, char* string_repr);
static int interface_build_routine(struct UsbDev* dev, MemoryService_C* mem_service,
    uint8_t* start, uint8_t* string_buffer, Interface** interfaces,
    Alternate_Interface** prev, unsigned int* prev_interface_number,
    unsigned int* interface_num);
static void endpoint_build_routine(struct UsbDev* dev, MemoryService_C* mem_service,
    unsigned int* endpoint_num, Alternate_Interface* prev, uint8_t* start);
__DECLARE_GET__(DeviceDescriptor, device_descriptor, device_desc,
  UsbDev*, static);
__DECLARE_GET__(Configuration*, active_configuration, active_config,
  UsbDev*, static);

uint8_t address = 0x01;

const uint8_t DEFAULT_STATE = 0x01;
const uint8_t ADDRESS_STATE = 0x02;
const uint8_t CONFIGURED_STATE = 0x03;

MemoryService_C* __DEV_MEMORY(UsbDev* dev) {
  __DEV_MEMORY__(dev, m); return m;
}

static inline void __transition_to_address_state(UsbDev* dev) {
  dev->address = address;
  dev->state = ADDRESS_STATE;
  address++;
}

static inline void __transition_to_config_state(UsbDev* dev) {
  dev->state = CONFIGURED_STATE;
}

static inline void __set_max_packet(UsbDev* dev, DeviceDescriptor* dev_desc) {
  dev->max_packet_size = dev_desc->bMaxPacketSize0;
}

static inline int8_t __has_endpoints(UsbDev* dev, Alternate_Interface* alt_itf){
  return __IF_EXT__(alt_itf->alternate_interface_desc.bInterfaceNumber == 0, -1, 1);
}

static inline Endpoint* __get_first_endpoint(UsbDev* dev, Alternate_Interface* alt_itf){
  return __IF_EXT__(__IS_NEG_ONE__(dev->__has_endpoints(dev, alt_itf)),
    (void*)0, alt_itf->endpoints[0]);
}

static inline int8_t __match_endpoint(UsbDev* dev, Endpoint* e, uint8_t endpoint_number){
  __IF_RET_SELF__(e->endpoint_desc.bEndpointAddress == endpoint_number,
      __RET_S__);
  return __RET_E__;
}

static inline Endpoint* __get_endpoint(UsbDev* dev, Alternate_Interface* alt_interface,
  uint8_t endpoint_number){
  __FOR_RANGE__(i, int, 0, alt_interface->alternate_interface_desc.bNumEndpoints){
    if(__match_endpoint(dev, alt_interface->endpoints[i], endpoint_number)){
      return alt_interface->endpoints[i];
    }
  }
  return (void*)0;
}

static inline Interface* __build_interface(UsbDev* dev, Alternate_Interface* alt_interface,
  char* interface_description, MemoryService_C* m) {
  Interface* interface = __ALLOC_KERNEL_MEM__(m, Interface, sizeof(Interface));
  interface->active = 0;
  interface->driver = 0;
  interface->active_interface = alt_interface;
  interface->alternate_interfaces = alt_interface;
  interface->interface_description = interface_description;

  return interface;
}

static inline void __set_config_as_active(UsbDev* dev, Configuration* configuration, 
  uint8_t* sel) {
  if (!(*sel)) {
      dev->active_config = configuration;
      *sel = 1;
  }
}

static inline void __retrieve_length_from_config(UsbDev* dev, 
  uint16_t* total_len, uint8_t* desc_len, 
  ConfigurationDescriptor* config_descriptor) {
  *total_len = config_descriptor->wTotalLength;
  *desc_len = config_descriptor->bLength;
}

static inline void __set_start(UsbDev* dev, uint8_t* config_buffer, 
  uint8_t* desc_len, uint8_t** start) {
  *start = config_buffer + *desc_len;
}

static inline void __set_end(UsbDev* dev, uint8_t* config_buffer, 
  uint16_t* tot_len, uint8_t** end) {
  *end = config_buffer + *tot_len;
}

static inline uint8_t __is_interface(UsbDev* dev, uint8_t* start) {
  return __IF_EXT__((*(start + 1) == INTERFACE), 1, 0);
}

static inline uint8_t __is_hid(UsbDev* dev, uint8_t* start) {
  return __IF_EXT__((*(start + 1) == HID), 1, 0);
}

static inline uint8_t __is_endpoint(UsbDev* dev, uint8_t* start) {
  return __IF_EXT__((*(start + 1) == ENDPOINT), 1, 0);
}

static inline void __request_dev_descriptor(UsbDev* dev, 
  UsbDeviceRequest* device_req, DeviceDescriptor* device_descriptor, 
  unsigned int len, uint8_t flag) {
  __STRUCT_CALL__(dev, request_build, device_req, DEVICE_TO_HOST, GET_DESCRIPTOR, 
    DEVICE, 0, 8, 0, len);
  __STRUCT_CALL__(dev, request, device_req, device_descriptor, PRIORITY_QH_8, 0,
               &request_callback, CONTROL_INITIAL_STATE | flag);
} 

static inline void __request_config_descriptor(UsbDev* dev, 
  UsbDeviceRequest* device_req, ConfigurationDescriptor* config_descriptor, 
  uint16_t configuration_index, unsigned int len) {
  __STRUCT_CALL__(dev, request_build, device_req, DEVICE_TO_HOST, GET_DESCRIPTOR,
    CONFIGURATION, configuration_index, 8, 0, len);
  __STRUCT_CALL__(dev, request, device_req, config_descriptor, PRIORITY_QH_8, 0,
    &request_callback, CONTROL_INITIAL_STATE);
}

static inline void __request_whole_config(UsbDev* dev, UsbDeviceRequest* device_req, 
  uint8_t* config_buffer, uint16_t configuration_index, unsigned int len) {
  __STRUCT_CALL__(dev, request_build, device_req, DEVICE_TO_HOST, GET_DESCRIPTOR,
    CONFIGURATION, configuration_index, 8, 0, len);
  __STRUCT_CALL__(dev, request, device_req, config_buffer, PRIORITY_QH_8, 0, 
    &request_callback, CONTROL_INITIAL_STATE);
}

static inline void __request_set_address(UsbDev* dev, 
  UsbDeviceRequest* device_req, uint8_t addr) {
  __STRUCT_CALL__(dev, request_build, device_req, HOST_TO_DEVICE, 
    SET_ADDRESS, 0, addr, 0, 0, 0);
  __STRUCT_CALL__(dev, request, device_req, 0, PRIORITY_QH_8, 0, 
    &request_callback, CONTROL_INITIAL_STATE);
}

static inline void __request_set_config(UsbDev* dev, UsbDeviceRequest* device_req) {
  __STRUCT_CALL__(dev, request_build, device_req, HOST_TO_DEVICE, SET_CONFIGURATION, 
    0, dev->active_config->config_desc.bConfigurationValue, 0, 0, 0);
  __STRUCT_CALL__(dev, request, device_req, 0, PRIORITY_QH_8, 0, &request_callback,
    CONTROL_INITIAL_STATE);
}

static inline void __request_string_descriptor(UsbDev* dev, 
  UsbDeviceRequest* device_req, uint8_t* string_buffer, 
  uint16_t index, uint16_t lang_id, unsigned int len) {
  __STRUCT_CALL__(dev, request_build, device_req, DEVICE_TO_HOST, GET_DESCRIPTOR, 
    STRING, index, 8, lang_id, len);
  __STRUCT_CALL__(dev, request, device_req, string_buffer, PRIORITY_QH_8, 0, &request_callback,
    CONTROL_INITIAL_STATE);
}

static inline void __request_set_protocol(UsbDev* dev, UsbDeviceRequest* device_req,
  Interface* interface, callback_function callback) {
  __STRUCT_CALL__(dev, request_build, device_req, 
    HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_INTERFACE, SET_PROTOCOL, 0, 
    USE_REPORT_PROTOCOL, 0, 
    interface->active_interface->alternate_interface_desc.bInterfaceNumber, 0);
  __STRUCT_CALL__(dev, request, device_req, 0, PRIORITY_QH_8, 0, callback,
    CONTROL_INITIAL_STATE);
}

static inline void __request_set_idle(UsbDev* dev, UsbDeviceRequest* device_req,
  Interface* interface){
  __STRUCT_CALL__(dev, request_build, device_req, 
    HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_INTERFACE, SET_IDLE, 0, 0, 0,
    interface->active_interface->alternate_interface_desc.bInterfaceNumber, 0);
  __STRUCT_CALL__(dev, request, device_req, 0, PRIORITY_QH_8, 0, 
    &request_callback, 0);
}

static inline void __request_set_report(UsbDev* dev, UsbDeviceRequest* device_req,
  Interface* interface, uint8_t type, unsigned int len, void* data, 
  callback_function callback) {
  __STRUCT_CALL__(dev, request_build, device_req, 
    HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_INTERFACE, SET_REPORT, type, 0, 
    8, interface->active_interface->alternate_interface_desc.bInterfaceNumber, len);
  __STRUCT_CALL__(dev, request, device_req, data, PRIORITY_QH_8, 0, callback,
    CONTROL_INITIAL_STATE);
}

static inline void __request_reset_bulk_only(UsbDev* dev, UsbDeviceRequest* device_req,
  Interface* interface, callback_function callback){
  __STRUCT_CALL__(dev, request_build, device_req, 
    HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_INTERFACE,
    RESET_BULK_ONLY_DEVICE, 0, 0, 0,
    interface->active_interface->alternate_interface_desc.bInterfaceNumber, 0);
  __STRUCT_CALL__(dev, request, device_req, 0, PRIORITY_8, 0, callback, 
    CONTROL_INITIAL_STATE);
}

static inline void __request_get_max_logic_unit_number(UsbDev* dev, 
  UsbDeviceRequest* device_req, Interface* interface, void* data,
  callback_function callback){
  __STRUCT_CALL__(dev, request_build, device_req, 
    DEVICE_TO_HOST | TYPE_REQUEST_CLASS | RECIPIENT_INTERFACE, GET_MAX_LUN, 0, 0, 0,
    interface->active_interface->alternate_interface_desc.bInterfaceNumber, 1);
  __STRUCT_CALL__(dev, request, device_req, data, PRIORITY_8, 0, callback,
    CONTROL_INITIAL_STATE);
}

static inline void __request_get_descriptor(UsbDev* dev, UsbDeviceRequest* device_req,
  void* data, unsigned int len, callback_function callback){
  __STRUCT_CALL__(dev, request_build, device_req,
    DEVICE_TO_HOST | TYPE_REQUEST_CLASS | RECIPIENT_DEVICE,
    GET_DESCRIPTOR, 0x2900, 0, 0, 0, len);
  __STRUCT_CALL__(dev, request, device_req, data, PRIORITY_8, 0, callback,
    CONTROL_INITIAL_STATE);
}

static inline void __request_get_req_status(UsbDev* dev, UsbDeviceRequest* device_req,
  void* data, unsigned int len, callback_function callback){
  dev->request_build(dev, device_req,
                     DEVICE_TO_HOST | TYPE_REQUEST_STANDARD | RECIPIENT_DEVICE,
                     GET_STATUS, 0, 0, 0, 0, len);
  dev->request(dev, device_req, data, PRIORITY_8, 0, callback,
               CONTROL_INITIAL_STATE);
}

static inline void __request_set_feature(UsbDev* dev, UsbDeviceRequest* device_req,
  uint16_t feature_value, uint16_t port, callback_function callback){
  dev->request_build(dev, device_req,
                     HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_OTHER,
                     SET_FEATURE, 0, feature_value, 0, port, 0);
  dev->request(dev, device_req, 0, PRIORITY_8, 0, callback, CONTROL_INITIAL_STATE);
}

static inline void __request_clear_feature(UsbDev* dev, UsbDeviceRequest* device_req,
  uint16_t feature_value, uint16_t port, callback_function callback){
  dev->request_build(dev, device_req,
                     HOST_TO_DEVICE | TYPE_REQUEST_CLASS | RECIPIENT_OTHER,
                     CLEAR_FEATURE, 0, feature_value, 0, port, 0);
  dev->request(dev, device_req, 0, PRIORITY_8, 0, callback, CONTROL_INITIAL_STATE);
}

static inline void __request_switch_alt_setting(UsbDev* dev, UsbDeviceRequest* device_req,
  Interface* interface, Alternate_Interface* alt_itf) {
  dev->request_build(dev, device_req, HOST_TO_DEVICE | RECIPIENT_INTERFACE,
                         SET_INTERFACE, 0, alt_itf->alternate_interface_desc.bAlternateSetting, 
                         0, interface->active_interface->alternate_interface_desc.bInterfaceNumber,
                         0);
  dev->request(dev, device_req, 0, PRIORITY_QH_8, 0, &request_callback, CONTROL_INITIAL_STATE);
  if(dev->error_while_transfering) return;
  interface->active_interface = alt_itf;
}

static inline void __request_switch_config(UsbDev* dev, UsbDeviceRequest* device_req,
  int configuration, callback_function callback) {
  dev->request_build(dev, device_req, HOST_TO_DEVICE, SET_CONFIGURATION, 0,
                         configuration, 0, 0, 0);
  dev->request(dev, device_req, 0, PRIORITY_QH_8, 0, callback, 0);
}

static inline uint8_t __get_alternate_settings(UsbDev* dev, Interface* interface){
  Alternate_Interface* alt_itf = interface->alternate_interfaces;
  uint8_t num_settings = 0;
  while(__NOT_NULL__(alt_itf)){
    num_settings++; alt_itf = alt_itf->next;
  }
  return num_settings;
}

static inline uint8_t __transfer_type(UsbDev* dev, Endpoint* e){
  return e->endpoint_desc.bmAttributes & TRANSFER_TYPE_MASK;
}

static inline int __is_transfer_type(UsbDev* dev, Endpoint* e, uint8_t transfer_type){
  return __IF_EXT__((e->endpoint_desc.bmAttributes & TRANSFER_TYPE_MASK) ==
        transfer_type, 1, 0);
}

static inline int __is_interrupt_type(UsbDev* dev, Endpoint* e){
  return __STRUCT_CALL__(dev, __is_transfer_type, e, TRANSFER_TYPE_INTERRUPT);
}

static inline int __is_bulk_type(UsbDev* dev, Endpoint* e){
  return __STRUCT_CALL__(dev, __is_transfer_type, e, TRANSFER_TYPE_BULK);
}

static inline int __is_control_type(UsbDev* dev, Endpoint* e){
  return __STRUCT_CALL__(dev, __is_transfer_type, e, TRANSFER_TYPE_CONTROL);
}

static inline uint16_t __max_payload(UsbDev* dev, Endpoint* e) {
  return e->endpoint_desc.wMaxPacketSize & WMAX_PACKET_SIZE_MASK;
}

static inline uint8_t __endpoint_number(UsbDev* dev, Endpoint* e) {
  return e->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;
}

static inline uint8_t __endpoint_default_or_not(UsbDev* dev, Endpoint* e) {
  return __IF_EXT__((__IS_NULL__(e)), 0, __STRUCT_CALL__(dev, 
    __endpoint_number, e));
}

static inline uint8_t __direction(UsbDev* dev, Endpoint* e) {
  return e->endpoint_desc.bEndpointAddress & DIRECTION_MASK;
}

static inline uint8_t __is_direction(UsbDev* dev, Endpoint* e, uint8_t direction){
  return __IF_EXT__((__STRUCT_CALL__(dev, __direction, e) == direction), 1, 0);
}

static inline uint8_t __is_direction_in(UsbDev* dev, Endpoint* e) {
  return __STRUCT_CALL__(dev, __is_direction, e, DIRECTION_IN);
}

static inline uint8_t __is_direction_out(UsbDev* dev, Endpoint* e) {
  return __STRUCT_CALL__(dev, __is_direction, e, DIRECTION_OUT);
}

static inline uint8_t __is_x_to_y(UsbDev* dev, UsbDeviceRequest* dev_req,
  uint8_t x_to_y){
  return __IF_EXT__(((dev_req->bmRequestType & DEVICE_TO_HOST) == x_to_y),
    1, 0);
}

static inline uint8_t __is_device_to_host(UsbDev* dev, UsbDeviceRequest* dev_req) {
  return __STRUCT_CALL__(dev, __is_x_to_y, dev_req, DEVICE_TO_HOST);
}

static inline uint8_t __is_host_to_device(UsbDev* dev, UsbDeviceRequest* dev_req) {
  return __STRUCT_CALL__(dev, __is_x_to_y, dev_req, HOST_TO_DEVICE);
}

static inline void __build_dev(UsbDev* dev, uint8_t speed, uint8_t port,
                               uint8_t level, uint8_t removable, uint8_t root_port,
                               uint8_t dev_num, void* controller){
  UsbController* casted_controller = __CAST__(UsbController*, controller);
  __MEM_SERVICE__(casted_controller->mem_service, mem_service);
  dev->device_request_map_io = __MAP_IO_KERNEL__(mem_service, uint8_t, PAGE_SIZE);

  __mem_set(dev->device_request_map_io, PAGE_SIZE, 0);
  __mem_set(dev->device_request_map_io_bitmap,
          PAGE_SIZE / sizeof(UsbDeviceRequest), 0);

  __NEW__(mem_service, Mutex_C, sizeof(Mutex_C), mutex, new_mutex, new_mutex);
  __NEW__(mem_service, Logger_C, sizeof(Logger_C), logger, new_logger, new_logger,
    USB_DEVICE_LOGGER_TYPE, LOGGER_LEVEL_DEBUG);

  dev->device_mutex = mutex;
  dev->device_logger = logger;

  dev->speed = speed;
  dev->port = port;
  dev->address = 0;
  dev->max_packet_size = 8; // default
  dev->level = level;
  dev->removable = removable;
  dev->rootport = root_port;
  dev->dev_num = dev_num;
  dev->max_down_stream = 0;
  dev->downstream_count = 0;
  dev->downstream_devs = 0;
  dev->active_config = 0;
  dev->supported_configs = 0;
  dev->controller = controller;
  dev->state = DEFAULT_STATE;
  dev->device_logger = 0;
  dev->error_while_transfering = 0;
  dev->lang_ids = __ALLOC_KERNEL_MEM__(mem_service, uint16_t, 
    SUPPORTED_LANGID_SIZE + 1);
  dev->lang_id = 0;
  dev->manufacturer = "";
  dev->product = "";
  dev->serial_number = "";
  dev->l_e.l_e = 0;
  dev->l_e_driver.l_e = 0;

  __INIT_DEV__(dev);
}

// root_port = tree level
// port = downstream port level
// dev_num = dev number in that tree
void new_usb_device(struct UsbDev *dev, uint8_t speed, uint8_t port,
                    uint8_t level, uint8_t removable, uint8_t root_port,
                    uint8_t dev_num, void *controller, 
                    uint8_t supress_flag) {
  __build_dev(dev, speed, port, level, removable, root_port, dev_num, controller);
  __DEV_MEMORY__(dev, mem_service);

  int map_io_offset = 0;
  uint8_t *map_io_buffer = __MAP_IO_KERNEL__(mem_service, uint8_t, PAGE_SIZE);
      
  __DEV_IO__ASSIGN__(map_io_buffer, map_io_offset, DeviceDescriptor, device_descriptor);
  __DEV_IO__ASSIGN__(map_io_buffer, map_io_offset, ConfigurationDescriptor, 
    config_descriptor);
  __DEV_IO_ASSIGN_SIZE__(map_io_buffer, map_io_offset, uint8_t, config_buffer, 
    sizeof(uint8_t) * CONFIG_BUFFER_SIZE);
  __DEV_IO_ASSIGN_SIZE__(map_io_buffer, map_io_offset, uint8_t, string_buffer,
    sizeof(uint8_t) * STRING_BUFFER_SIZE);

  __IF_COND__(__STRUCT_CALL__(dev, process_device_descriptor, device_descriptor, 
    8, supress_flag) == __RET_E__) {
    __IF_RET__(supress_flag);
    __DEV_LOG_ERROR__(dev, "Aborting configuration of device due to error "
                                "while handle the device descriptor ...");
  }
  __STRUCT_CALL__(dev, __set_max_packet, device_descriptor);

  __PROCESS_ROUTINE__(dev, set_address, "Aborting configuration of device due to error "
    "while setting the address ...", address);

  __STRUCT_CALL__(dev, __transition_to_address_state);

  __PROCESS_ROUTINE__(dev, handle_lang, "Aborting configuration of device due to error "
    "while handling the langs ...", string_buffer);
  __PROCESS_ROUTINE__(dev, handle_dev, "Aborting configuration of device due to error "
    "while handling device spefic requests ...", string_buffer, device_descriptor);
  __PROCESS_ROUTINE__(dev, handle_configuration, "Aborting configuration of device due to error while handling internal "
    "configuration of the device ...", string_buffer, config_buffer, config_descriptor,
      device_descriptor->bNumConfigurations);
  __PROCESS_ROUTINE__(dev, set_configuration, "Aborting configuration of device due to error "
    "while setting the configration ...", dev->active_config->config_desc.bConfigurationValue);

  __STRUCT_CALL__(dev, __transition_to_config_state);

  __STRUCT_CALL__(__CAST__(__UHC__*, dev->controller), add_device, dev);
  __STRUCT_CALL__(mem_service, unmap, __PTR_TYPE__(uint32_t, map_io_buffer));
}

static void add_downstream_device(UsbDev *dev, UsbDev *downstream_dev) {
  __IF_RET__(dev->downstream_count == dev->max_down_stream);
  dev->downstream_devs[dev->downstream_count] = downstream_dev;
  dev->downstream_count = dev->downstream_count + 1;
}

static void add_downstream(UsbDev *dev, uint8_t downstream_ports) {
  __IF_RET__(!downstream_ports);
  __DEV_MEMORY__(dev, m);
  UsbDev **down_d = 
    __ALLOC_KERNEL_MEM__(m, UsbDev*, sizeof(UsbDev *) * downstream_ports);
  dev->downstream_devs = down_d;
  dev->max_down_stream = downstream_ports;
  dev->downstream_count = 0;
}

static void delete_usb_dev(UsbDev *dev) {
  __DEV_MEMORY__(dev, m);
  if (dev->downstream_devs != (void *)0)
    m->freeKernelMemory_c(m, dev->downstream_devs, 0);
  m->freeKernelMemory_c(m, dev->device_mutex, 0);
  m->unmap(m, (uint32_t)(uintptr_t)dev->device_request_map_io);
  m->freeKernelMemory_c(m, dev->lang_ids, 0);
  m->freeKernelMemory_c(m, dev->device_logger, 0);
  dev->free_usb_dev_strings(dev);
  dev->free_usb_dev_configs(dev);
  m->freeKernelMemory_c(m, dev, 0);
}

static void free_usb_dev_strings(UsbDev *dev) {
  __DEV_MEMORY__(dev, m);
  if (dev->manufacturer[0] != '\0')
    m->freeKernelMemory_c(m, dev->manufacturer, 0);
  if (dev->product[0] != '\0')
    m->freeKernelMemory_c(m, dev->product, 0);
  if (dev->serial_number[0] != '\0')
    m->freeKernelMemory_c(m, dev->serial_number, 0);
}

static void free_usb_dev_configs(UsbDev *dev) {
  __DEV_MEMORY__(dev, m);
  Configuration **configs = dev->supported_configs;
  if (configs == (void *)0)
    return;

  for (int i = 0; i < dev->device_desc.bNumConfigurations; i++) {
    Configuration *config = configs[i];
    if (config->config_description[0] != '\0')
      m->freeKernelMemory_c(m, config->config_description, 0);
    int num_interfaces = config->config_desc.bNumInterfaces;
    dev->free_usb_dev_interfaces(dev, config->interfaces, num_interfaces);
  }
  m->freeKernelMemory_c(m, configs, 0);
}

static void free_usb_dev_interfaces(UsbDev *dev, Interface **interfaces,
                             int num_interfaces) {
  __DEV_MEMORY__(dev, m);
  for (int i = 0; i < num_interfaces; i++) {
    Interface *itf = interfaces[i];
    Alternate_Interface *alt_itf = itf->alternate_interfaces;
    if (itf->interface_description[0] != '\0')
      m->freeKernelMemory_c(m, itf->interface_description, 0);
    while (alt_itf != (void *)0) {
      dev->free_usb_dev_endpoints(
          dev, alt_itf->endpoints,
          alt_itf->alternate_interface_desc.bNumEndpoints);
      Alternate_Interface *temp = alt_itf;
      alt_itf = alt_itf->next;
      m->freeKernelMemory_c(m, temp, 0);
    }
    m->freeKernelMemory_c(m, itf, 0);
  }
  m->freeKernelMemory_c(m, interfaces, 0);
}

static void free_usb_dev_endpoints(UsbDev *dev, Endpoint **endpoints,
                            int num_endpoints) {
  __DEV_MEMORY__(dev, m);
  for (int i = 0; i < num_endpoints; i++) {
    m->freeKernelMemory_c(m, endpoints[i], 0);
  }
  m->freeKernelMemory_c(m, endpoints, 0);
}

static char* string_build_routine(UsbDev* dev, unsigned int string_index,
  uint8_t* string_buffer, char* string_repr) {
  uint8_t s_len;
 
  __IF_RET_NULL__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, process_string_descriptor,
    string_buffer, string_index, dev->lang_id, string_repr, 1)));
  s_len = string_buffer[0];
  __IF_RET_NULL__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, process_string_descriptor,
    string_buffer, string_index, dev->lang_id, string_repr, s_len)));
  return __STRUCT_CALL__(dev, build_string, s_len, string_buffer);
}

static int interface_build_routine(UsbDev* dev, MemoryService_C* mem_service,
  uint8_t* start, uint8_t* string_buffer, Interface** interfaces,
  Alternate_Interface** prev, unsigned int* prev_interface_number,
  unsigned int* interface_num) {
  char *ascii_string;

  __ALLOC_KERNEL_MEM_S__(mem_service, Alternate_Interface, alt_interface);
  alt_interface->next = 0;

  __TYPE_CAST__(InterfaceDescriptor*, interface_desc, start);

  __STRING_ROUTINE__(dev, "iInterface", interface_desc->iInterface, string_buffer,
    ascii_string);

  alt_interface->alternate_interface_desc = *interface_desc;

  if (interface_desc->bInterfaceNumber == *prev_interface_number) {
    (*prev)->next = alt_interface;
    (*prev) = (*prev)->next;
  } else {
    Interface* interface = __STRUCT_CALL__(dev, __build_interface, 
      alt_interface, ascii_string, mem_service);
    *prev = alt_interface;
    interfaces[(*interface_num)++] = interface;
  }
  
  *prev_interface_number = interface_desc->bInterfaceNumber;
  alt_interface->endpoints = __ALLOC_KERNEL_MEM__(mem_service, Endpoint*,
    sizeof(struct Endpoint *) * interface_desc->bNumEndpoints);
  return __RET_S__;
}

static void endpoint_build_routine(UsbDev* dev, MemoryService_C* mem_service,
  unsigned int* endpoint_num, Alternate_Interface* prev, uint8_t* start) {
  __TYPE_CAST__(EndpointDescriptor*, endpoint_desc, start);
  __ALLOC_KERNEL_MEM_S__(mem_service, Endpoint, endpoint);
  endpoint->endpoint_desc = *endpoint_desc;
  prev->endpoints[(*endpoint_num)++] = endpoint;
}

static int handle_interface(UsbDev *dev, Configuration *configuration,
                     uint8_t *string_buffer, uint8_t *start, uint8_t *end,
                     uint8_t num_interfaces) {
  __DEV_MEMORY__(dev, mem_service);
  __ALLOC_KERNEL_MEM_T__(mem_service, Interface*, interfaces, num_interfaces);
  configuration->interfaces = interfaces;

  Alternate_Interface *prev;
  unsigned int interface_num = 0;
  unsigned int endpoint_num = 0;
  unsigned int prev_interface_number = -1;
  
  while (start < end) {
    if (__STRUCT_CALL__(dev, __is_interface, start)) {
      __STRUCT_CALL__(dev, interface_build_routine, mem_service, start, 
        string_buffer, interfaces, &prev, &prev_interface_number, &interface_num);
      endpoint_num = 0;
    }
    //else if (__STRUCT_CALL__(dev, __is_hid, start)) {} -> use in class specific driver
    else if (__STRUCT_CALL__(dev, __is_endpoint, start)) {
     __STRUCT_CALL__(dev, endpoint_build_routine, mem_service, &endpoint_num, 
      prev, start);
    }
    start += *(start);
  }
  return __RET_S__;
}

static int handle_configuration(UsbDev *dev, uint8_t *string_buffer,
                         uint8_t *config_buffer,
                         ConfigurationDescriptor *config_descriptor,
                         uint8_t num_configurations) {
  __DEV_MEMORY__(dev, mem_service);
  uint16_t total_len;
  uint8_t desc_len;
  uint8_t *start, *end;
  char *ascii_string;
  uint8_t config_sel = 0, current_entry = 0;

  __ALLOC_KERNEL_MEM_T__(mem_service, Configuration*, configurations, 
    num_configurations);
  dev->supported_configs = configurations;
  // default to config0, interface0
  __FOR_RANGE__(i, int, 0, num_configurations) {
    // request first 4 bytes of each config desc (wTotalLength)
    __IF_RET_NEG__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, 
      process_configuration_descriptor, config_descriptor, i, 4)));
    __STRUCT_CALL__(dev, __retrieve_length_from_config, &total_len, 
      &desc_len, config_descriptor);
    __IF_CONTINUE__(total_len > CONFIG_BUFFER_SIZE);
    __IF_RET_NEG__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, 
      process_whole_configuration, config_buffer, i, total_len)));
    __ALLOC_KERNEL_MEM_S__(mem_service, Configuration, configuration);
    __TYPE_CAST__(ConfigurationDescriptor*, config_desc, config_buffer);
    configuration->config_description = "";
    __STRING_ROUTINE__(dev, "iConfiguration", config_desc->iConfiguration,
      string_buffer, ascii_string);

    __STRUCT_CALL__(dev, __set_config_as_active, configuration, &config_sel);
    __STRUCT_CALL__(dev, __set_start, config_buffer, &desc_len, &start);
    __STRUCT_CALL__(dev, __set_end, config_buffer, &total_len, &end);
    configuration->config_desc = *config_desc;
    __STRUCT_CALL__(dev, handle_interface, configuration, string_buffer, 
      start, end, config_desc->bNumInterfaces);
    configurations[current_entry++] = configuration;
  }
  return __RET_S__;
}

static int handle_lang(UsbDev *dev, uint8_t *string_buffer) {
  uint8_t s_len;

  __IF_RET_NEG__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, process_string_descriptor,
    string_buffer, 0, 0, "langs", 1)));
  s_len = string_buffer[0];
  __IF_RET_NEG__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, process_string_descriptor,
    string_buffer, 0, 0, "langs", s_len)));

  __STRUCT_CALL__(dev, process_lang_ids, string_buffer, s_len);

  return __RET_S__;
}

static int handle_dev(UsbDev *dev, uint8_t *string_buffer,
               DeviceDescriptor *device_descriptor) {
  char *ascii_string;
  __IF_RET_NEG__(__IS_NEG_ONE__(__STRUCT_CALL__(dev,
     process_device_descriptor, device_descriptor, sizeof(DeviceDescriptor), 0)));

  dev->device_desc = *device_descriptor;

  __STRING_ROUTINE__(dev, "iManufacturer", device_descriptor->iManufacturer, 
    string_buffer, ascii_string);
  dev->manufacturer = ascii_string;
  __STRING_ROUTINE__(dev, "iProduct", device_descriptor->iProduct, 
    string_buffer, ascii_string);
  dev->product = ascii_string;
  __STRING_ROUTINE__(dev, "iSerialNumber", device_descriptor->iSerialNumber,
    string_buffer, ascii_string);
  dev->serial_number = ascii_string;

  return __RET_S__;
}

static char *build_string(UsbDev *dev, int len, uint8_t *string_buffer) {
  __DEV_MEMORY__(dev, mem_service);

  uint8_t *start_string_b = string_buffer + 2;
  uint8_t *end_string_b = string_buffer + len;

  char *ascii_string =
      (char *)mem_service->allocateKernelMemory_c(mem_service, len - 1, 0);
  int ascii_index = 0;

  while (start_string_b < end_string_b) {
    ascii_string[ascii_index++] = *((uint8_t *)((uint16_t *)start_string_b));
    start_string_b += 2;
  }
  ascii_string[ascii_index] = '\0';

  return ascii_string;
}

static void process_lang_ids(UsbDev *dev, uint8_t *string_buffer, int s_len) {
  uint8_t *start_string_b = string_buffer + 2;
  uint8_t *end_string_b = string_buffer + s_len;

  uint16_t *lang_id;
  int lang_id_index = 0, lang_sel = 0;

  while (start_string_b < end_string_b) {
    lang_id = (uint16_t *)start_string_b;
    if (!lang_sel) {
      dev->lang_id = *lang_id;
      lang_sel = 1;
    }
    if (lang_id_index < SUPPORTED_LANGID_SIZE) {
      *(dev->lang_ids + lang_id_index) = *lang_id;
      *(dev->lang_ids + lang_id_index + 1) = 0;
      lang_id_index++;
    }
    start_string_b += 2;
  }
}

static void request_callback(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER) {
    dev->error_while_transfering = 1;
  } else if (status & S_TRANSFER) {
    dev->error_while_transfering = 0;
  }
}

static void request_build(UsbDev *dev, UsbDeviceRequest *device_request,
                   int8_t rq_type, int8_t rq, int16_t value_high,
                   int16_t value_low, int16_t shift, int16_t index,
                   int16_t len) {
  device_request->bmRequestType = rq_type;
  device_request->bRequest = rq;
  device_request->wValue = value_high << shift | value_low;
  device_request->wIndex = index;
  device_request->wLength = len;
}

static void request(UsbDev *dev, UsbDeviceRequest *device_request, void *data,
             uint8_t priority, Endpoint *endpoint, callback_function callback,
             uint8_t flags) {
  ((UsbController *)dev->controller)
      ->control_entry_point(dev, device_request, data, priority, endpoint,
                            callback, flags);
}

// update struct Interface + active field in alternate setting
static int request_switch_alternate_setting(UsbDev *dev, Interface *interface,
                                     int setting) {
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));
  Alternate_Interface *current = interface->alternate_interfaces;
  while (__NOT_NULL__(current)) {
    if (current->alternate_interface_desc.bAlternateSetting == setting) {
      __REQUEST_ROUTINE_CALLBACK__(dev, __request_switch_alt_setting, interface,
        current);
    }
    current = current->next;
  }
  return __RET_N__;
}

// switch active config field in UsbDev
static int request_switch_configuration(UsbDev *dev, int configuration,
                                 callback_function callback) {
  unsigned int config_count = dev->device_desc.bNumConfigurations;
  Configuration **allowed_configs = dev->supported_configs;
  __IF_RET_NEG__(config_count < configuration);
  __FOR_RANGE__(i, int, 0, config_count) {
    Configuration *config = allowed_configs[i];
    if (config->config_desc.bConfigurationValue == configuration) {
      __REQUEST_ROUTINE_CALLBACK__(dev, __request_switch_config, configuration, 
        callback);
      dev->active_config = allowed_configs[i];
      return __RET_S__;
    }
  }
  return __RET_N__;
}

static int set_address(UsbDev *dev, uint8_t address) {
  __REQUEST_ROUTINE__(dev, __request_set_address, 
    "Couldn't set address of device !", address);
}

static int process_device_descriptor(UsbDev *dev, DeviceDescriptor *device_descriptor,
                              unsigned int len, uint8_t supress_flag) {
  UsbDeviceRequest *request = dev->get_free_device_request(dev);
  uint8_t S_FLAG = 0;

  if (request == (void *)0)
    return -1;

  if(supress_flag) S_FLAG = SUPRESS_DEVICE_ERRORS;

  dev->request_build(dev, request, DEVICE_TO_HOST, GET_DESCRIPTOR, DEVICE, 0, 8,
                     0, len);
  dev->request(dev, request, device_descriptor, PRIORITY_QH_8, 0,
               &request_callback, CONTROL_INITIAL_STATE | S_FLAG);

  if (dev->error_while_transfering) {
    if(supress_flag) return -1;
    dev->device_logger->error_c(
        dev->device_logger, "Couldn't read device descriptor from device !");
    return -1;
  }

  return 1;
}

static int process_configuration_descriptor(UsbDev *dev,
                                     ConfigurationDescriptor *config_descriptor,
                                     uint16_t configuration_index,
                                     unsigned int len) {
  DeviceDescriptor device_descriptor = dev->device_desc;
  __IF_RET_NEG__(configuration_index > device_descriptor.bNumConfigurations);
  __IF_RET_NEG__(dev->state == DEFAULT_STATE);

  __REQUEST_ROUTINE__(dev, __request_config_descriptor, 
    "Couldn't read configuration descriptor from device !", config_descriptor,
    configuration_index, len);
}

static int process_whole_configuration(UsbDev *dev, uint8_t *config_buffer,
                                uint16_t configuration_index,
                                unsigned int len) {
  DeviceDescriptor device_descriptor = dev->device_desc;
  __IF_RET_NEG__(configuration_index > device_descriptor.bNumConfigurations);
  __IF_RET_NEG__(dev->state == DEFAULT_STATE);

  __REQUEST_ROUTINE__(dev, __request_whole_config, 
    "Couldn't read configuration descriptor from device !", config_buffer,
    configuration_index, len);
}

static int process_string_descriptor(UsbDev *dev, uint8_t *string_buffer,
                              uint16_t index, uint16_t lang_id, char *s,
                              unsigned int len) {
  __IF_RET_NEG__(lang_id != 0 && lang_id != dev->lang_id);
  __IF_RET_NEG__(__IS_NULL__(s));

  __REQUEST_ROUTINE__(dev, __request_string_descriptor, 
    "Couldn't read string descriptor!", string_buffer, index, lang_id, len);
}

static int set_configuration(UsbDev *dev, uint8_t configuration) {
  __IF_RET_NEG__(configuration > dev->active_config->config_desc.bNumInterfaces);

  __REQUEST_ROUTINE__(dev, __request_set_config, 
    "Couldn't set configuration of device !");
}

static int usb_dev_interface_lock(UsbDev *dev, Interface *interface, void *driver) {
  int interface_count = dev->active_config->config_desc.bNumInterfaces;
  Interface **interfaces = dev->active_config->interfaces;
  // dev->device_mutex->acquire_c(dev->device_mutex);
  int i_found = 0;
  __FOR_RANGE__(i, int, 0, interface_count){
    __IF_CUSTOM__(interfaces[i] == interface, i_found = 1);
  }
  __IF_CUSTOM__(!i_found, return E_INTERFACE_INV);
  __IF_CUSTOM__(interface->active, return E_INTERFACE_IN_USE);

  interface->active = 1;
  interface->driver = driver;
  // dev->device_mutex->release_c(dev->device_mutex);
  return __RET_S__;
}

static void usb_dev_free_interface(UsbDev *dev, Interface *interface) {
  interface->active = 0;
  interface->driver = 0;
}

UsbDeviceRequest *get_free_device_request(UsbDev *dev) {
  for (int i = 0; i < PAGE_SIZE / sizeof(UsbDeviceRequest); i++) {
    // dev->device_mutex->acquire_c(dev->device_mutex);
    if (dev->device_request_map_io_bitmap[i] == 0) {
      dev->device_request_map_io_bitmap[i] = 1;
      // dev->device_mutex->release_c(dev->device_mutex);
      return (UsbDeviceRequest *)(dev->device_request_map_io +
                                  (i * sizeof(UsbDeviceRequest)));
    }
    // dev->device_mutex->release_c(dev->device_mutex);
  }
  return (void *)0;
}

static void free_device_request(UsbDev *dev, UsbDeviceRequest *device_request) {
  for (int i = 0; i < PAGE_SIZE; i += sizeof(UsbDeviceRequest)) {
    // dev->device_mutex->acquire_c(dev->device_mutex);
    if ((dev->device_request_map_io + i) == (uint8_t *)device_request) {
      dev->device_request_map_io_bitmap[i / sizeof(UsbDeviceRequest)] = 0;
      // dev->device_mutex->release_c(dev->device_mutex);
      return;
    }
    // dev->device_mutex->release_c(dev->device_mutex);
  }
}

static int set_report(UsbDev *dev, Interface *interface, uint8_t type, void *data,
               unsigned int len, callback_function callback) {
  __IF_RET_NEG__(type != REQUEST_OUTPUT && type != REQUEST_INPUT);
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));

  __REQUEST_ROUTINE_CALLBACK__(dev, __request_set_report, interface, type,
    len, data, callback);
}

static int set_protocol(UsbDev *dev, Interface *interface, uint16_t protocol_value,
                 callback_function callback) {
  __IF_RET_NEG__(protocol_value != USE_BOOT_PROTOCOL &&
      protocol_value != USE_REPORT_PROTOCOL);
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));

  __REQUEST_ROUTINE_CALLBACK__(dev, __request_set_protocol, interface, callback);
}

static int set_idle(UsbDev *dev, Interface *interface) {
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));

  __REQUEST_ROUTINE__(dev, __request_set_idle, "Set idle request invalid ... ",
    interface);
}

static int reset_bulk_only(UsbDev *dev, Interface *interface,
                    callback_function callback) {
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));
  __REQUEST_ROUTINE_CALLBACK__(dev, __request_reset_bulk_only, interface, callback);
}

static int get_max_logic_unit_numbers(UsbDev *dev, Interface *interface, uint8_t *data,
                               callback_function callback) {
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));
  __REQUEST_ROUTINE_CALLBACK__(dev, __request_get_max_logic_unit_number, interface,
    data, callback);
}

static int get_descriptor(UsbDev *dev, Interface *interface, uint8_t *data,
                   unsigned int len, callback_function callback) {
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));
  __REQUEST_ROUTINE_CALLBACK__(dev, __request_get_descriptor, data, len, callback);
}

static int get_req_status(UsbDev *dev, Interface *interface, uint8_t *data,
                   unsigned int len, callback_function callback) {
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));
  __REQUEST_ROUTINE_CALLBACK__(dev, __request_get_req_status, data, len, callback);
}

static int set_feature(UsbDev *dev, Interface *interface, uint16_t feature_value,
                uint16_t port, callback_function callback) {
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));
  __REQUEST_ROUTINE_CALLBACK__(dev, __request_set_feature, feature_value, port, 
    callback);
}

static int clear_feature(UsbDev *dev, Interface *interface, uint16_t feature_value,
                  uint16_t port, callback_function callback) {
  __IF_RET_NEG__(!dev->contain_interface(dev, interface));
  __REQUEST_ROUTINE_CALLBACK__(dev, __request_clear_feature, feature_value, port, 
    callback);
}

// just support get descriptor, get configuration, get interface, get status,
// set interface
static void usb_dev_control(UsbDev *dev, Interface *interface, unsigned int pipe,
                     uint8_t priority, void *data, uint8_t *setup,
                     callback_function callback, uint8_t flags) {
  UsbDeviceRequest *device_req = (UsbDeviceRequest *)setup;
  __USB_DEV_CTL_ROUTINE__(dev, interface, data, callback, pipe, 
    device_req, data, priority, endpoints[i], callback, flags);
}

static void usb_dev_bulk(struct UsbDev *dev, Interface *interface, unsigned int pipe,
                  uint8_t priority, void *data, unsigned int len,
                  callback_function callback, uint8_t flags) {
  __USB_DEV_BULK_ROUTINE__(dev, interface, data, callback, pipe, endpoints[i], 
    data, len, priority, callback, flags);
}

static void usb_dev_interrupt(UsbDev *dev, Interface *interface, unsigned int pipe,
                       uint8_t priority, void *data, unsigned int len,
                       uint16_t interval, callback_function callback) {
  __USB_DEV_INTR_ROUTINE__(dev, interface, data, callback, pipe, endpoints[i], 
    data, len, priority, interval, callback);
}

static void usb_dev_iso(UsbDev* dev, Interface* interface, unsigned int pipe,
  uint8_t priority, void* data, unsigned int len, uint16_t interval, 
  callback_function callback){
  __USB_DEV_ISO_ROUTINE__(dev, interface, data, callback, pipe, endpoints[i],
    data, len, priority, interval, callback);
}

static int contain_interface(UsbDev *dev, Interface *interface) {
  Interface **interfaces = dev->active_config->interfaces;

  __FOR_RANGE__(i, int, 0, dev->active_config->config_desc.bNumInterfaces) {
    __IF_RET_POS__(interfaces[i] == interface);
  }
  return __RET_N__;
}

static int is_pipe_buildable(UsbDev* dev, Endpoint *endpoint, unsigned int pipe) {
  unsigned int type;
  unsigned int end_point;
  unsigned int direction;
  unsigned int reserved;

  type = (pipe & CONTROL_PIPE_MASK) >> 5;
  end_point = pipe & ENDPOINT_MASK;
  reserved = (pipe & 0x10) >> 4;
  direction = pipe & DIRECTION_MASK;

  return __STRUCT_CALL__(dev, __is_direction, endpoint, direction)     && 
         __STRUCT_CALL__(dev, __is_transfer_type, endpoint, type)      &&
         end_point == __STRUCT_CALL__(dev,__endpoint_number, endpoint) &&
         reserved == 0;
}

static int8_t support_bulk(UsbDev *dev, Interface *interface) {
  return __STRUCT_CALL__(dev, support_transfer_type, interface, TRANSFER_TYPE_BULK);
}

static int8_t support_isochronous(UsbDev *dev, Interface *interface) {
  return __STRUCT_CALL__(dev, support_transfer_type, interface, TRANSFER_TYPE_ISO);
}

static int8_t support_control(UsbDev *dev, Interface *interface) {
  return __STRUCT_CALL__(dev, support_transfer_type, interface, TRANSFER_TYPE_CONTROL);
}

static int8_t support_interrupt(UsbDev *dev, Interface *interface) {
  return __STRUCT_CALL__(dev, support_transfer_type, interface, TRANSFER_TYPE_INTERRUPT);
}

static int8_t support_transfer_type(UsbDev *dev, Interface *interface,
                             uint8_t transfer_type) {
  int8_t supports = 0;
  Endpoint **endpoints = interface->active_interface->endpoints;
  Alternate_Interface *active_interface = interface->active_interface;
  for (int i = 0; i < active_interface->alternate_interface_desc.bNumEndpoints; i++) {
    __IF_CUSTOM__(__STRUCT_CALL__(dev, __is_transfer_type, endpoints[i], 
      transfer_type), supports = 1);
  }
  return supports;
}

static void dump_device_desc(UsbDev *dev) {
  DeviceDescriptor dev_desc = dev->device_desc;
  char *message =
      "Dumping Device Descriptor :\n"
      "\tbLength : %u\n\tbType : %u\n\tbcdUsb : %u\n\tbDeviceClass : %u\n"
      "\tbDeviceSubClass : %u\n\tbDeviceProtocol : %u\n\tbMaxPacketSize : %u\n"
      "\tidVendor : %u\n\tidProduct : %u\n\tbcdDevice : %u\n\tiManufacturer : "
      "%u\n"
      "\tiProduct : %u\n\tiSerialNumber : %u\n\tbNumConfigs : %u\n";

  dev->device_logger->debug_c(
      dev->device_logger, message, dev_desc.bLength, dev_desc.bDescriptorType,
      dev_desc.bcdUSB, dev_desc.bDeviceClass, dev_desc.bDeviceSubClass,
      dev_desc.bDeviceProtocol, dev_desc.bMaxPacketSize0, dev_desc.idVendor,
      dev_desc.idProduct, dev_desc.bcdDevice, dev_desc.iManufacturer,
      dev_desc.iProduct, dev_desc.iSerialNumber, dev_desc.bNumConfigurations);
}

static void dump_configuration(UsbDev *dev) {
  ConfigurationDescriptor config_desc = dev->active_config->config_desc;
  char *message =
      "Config Descriptor : \n"
      "\tbLength : %u\n\tbType : %u\n\twTotalLength : %u\n"
      "\tbNumInterfaces : %u\n\tbConfigurationValue : %u\n"
      "\tiConfiguration : %u\n\tSelf-Power : %u\n\tRemote-Wakeup : %u\n"
      "\tbMaxPower : %u";

  dev->device_logger->debug_c(
      dev->device_logger, message, config_desc.bLength,
      config_desc.bDescriptorType, config_desc.wTotalLength,
      config_desc.bNumInterfaces, config_desc.bConfigurationValue,
      config_desc.iConfiguration, config_desc.bmAttributes & POWERED_MASK,
      config_desc.bmAttributes & REMOTE_WAKEUP_MASK, config_desc.bMaxPower);
}

static void dump_interface(struct UsbDev *dev, Interface *interface) {
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;

  char *message = "Interface : \n"
                  "\tactive : %u\n\tdriver : %x\n\tinterface-description : %s";

  dev->device_logger->debug_c(dev->device_logger, message, interface->active,
                              interface->driver,
                              interface->interface_description);

  message = "Interface Descriptor : \n"
            "\tbLength : %u\n\tbType : %u\n\tbInterfaceNumber : %u\n"
            "\tbAlternateSetting : %u\n\tbNumEndpoints : %u\n"
            "\tbInterfaceClass : %u\n\tbInterfaceSubClass : %u\n"
            "\tbInterfaceProtocol : %u\n\tiInterface : %u\n";

  dev->device_logger->debug_c(
      dev->device_logger, message, interface_desc.bLength,
      interface_desc.bDescriptorType, interface_desc.bInterfaceNumber,
      interface_desc.bAlternateSetting, interface_desc.bNumEndpoints,
      interface_desc.bInterfaceClass, interface_desc.bInterfaceSubClass,
      interface_desc.bInterfaceProtocol, interface_desc.iInterface);
}

static void dump_endpoints(struct UsbDev *dev, Interface *interface) {
  Endpoint **endpoints = interface->active_interface->endpoints;
  uint8_t endpoint_num =
      interface->active_interface->alternate_interface_desc.bNumEndpoints;
  for (int i = 0; i < endpoint_num; i++) {
    dev->dump_endpoint(dev, endpoints[i]);
  }
}

static void dump_endpoint(struct UsbDev *dev, Endpoint *endpoint) {
  EndpointDescriptor endpoint_desc = endpoint->endpoint_desc;
  char *message =
      "Endpoint Descriptor : \n"
      "\tbLength : %u\n\tbType : %u\n\tDirection : %u\n\tEndpoint : %u\n"
      "\tTransfer-Type : %u\n\twMaxPacketSize : %u\n\tbInterval : %u\n";

  dev->device_logger->debug_c(
      dev->device_logger, message, endpoint_desc.bLength,
      endpoint_desc.bDescriptorType,
      endpoint_desc.bEndpointAddress & DIRECTION_MASK,
      endpoint_desc.bEndpointAddress & ENDPOINT_MASK,
      endpoint_desc.bmAttributes & TRANSFER_TYPE_MASK,
      endpoint_desc.wMaxPacketSize, endpoint_desc.bInterval);
}

static void dump_device(struct UsbDev *dev) {
  char *message = "Usb-Device : \n"
                  "\tspeed : %u\n\tport : %u\n\taddress : %u\n\tstate : %s\n"
                  "\tlang-id : 0x%04x\n\tmanufacturer : %s\n\tproduct : %s\n"
                  "\tserial-number : %s";
  char *s = "";
  if (dev->state == DEFAULT_STATE) {
    s = "default-state";
  } else if (dev->state == ADDRESS_STATE) {
    s = "address-state";
  } else if (dev->state == CONFIGURED_STATE) {
    s = "config-state";
  }

  dev->device_logger->debug_c(
      dev->device_logger, message, dev->speed, dev->port, dev->address, s,
      dev->lang_id, dev->manufacturer, dev->product, dev->serial_number);
  dev->dump_device_desc(dev);
  dev->dump_configuration(dev);

  int interface_num = dev->active_config->config_desc.bNumInterfaces;
  for (int i = 0; i < interface_num; i++) {
    dev->dump_interface(dev, dev->active_config->interfaces[i]);
    dev->dump_endpoints(dev, dev->active_config->interfaces[i]);
  }
}