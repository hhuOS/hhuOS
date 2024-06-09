/***********
 * Usb Device listening at port 1,2
 * Supporting control, interrupt and isochronous transfers
 *
 ***********/

#ifndef USB_DEVICE_INCLUDE
#define USB_DEVICE_INCLUDE

#include "../interfaces/LoggerInterface.h"
#include "../interfaces/MutexInterface.h"
#include "../interfaces/SystemInterface.h"
#include "stdint.h"
#include "../utility/Utils.h"
#include "requests/UsbDescriptors.h"
#include "requests/UsbRequests.h"
#include "data/UsbDev_Data.h"

#define BULK_INITIAL_STATE 0x01
#define CONTROL_INITIAL_STATE 0x02
#define SUPRESS_DEVICE_ERRORS 0x04
#define ISO_EXT 0x08
#define NO_ACTIVATE 0x10

#define __DEV_MEMORY__(dev, name) \
  UsbController* c = __CAST__(UsbController*, dev->controller); \
  __MEM_SERVICE__(c->mem_service, name)
  
#define __ASSIGN_MEM__(type, name, buffer, off) \
  type* name = (type*)(buffer + off)

#define __DEV_IO__ASSIGN__(buffer, off, type, name) \
  __ASSIGN_MEM__(type, name, buffer, off); \
  off += sizeof(type)

#define __DEV_IO_ASSIGN_SIZE__(buffer, off, type, name, size) \
  __ASSIGN_MEM__(type, name, buffer, off); \
  off += size

#define __INIT_DEV__(name) \
  __ENTRY__(name, request_build) = &request_build; \
  __ENTRY__(name, request) = &request; \
  __ENTRY__(name, usb_dev_control) = &usb_dev_control; \
  __ENTRY__(name, usb_dev_interrupt) = &usb_dev_interrupt; \
  __ENTRY__(name, usb_dev_bulk) = &usb_dev_bulk; \
  __ENTRY__(name, usb_dev_iso) = &usb_dev_iso; \
  __ENTRY__(name, usb_dev_interface_lock) = &usb_dev_interface_lock; \
  __ENTRY__(name, usb_dev_free_interface) = &usb_dev_free_interface; \
  __ENTRY__(name, request_callback) = &request_callback; \
  __ENTRY__(name, is_pipe_buildable) = &is_pipe_buildable; \
  __ENTRY__(name, contain_interface) = &contain_interface; \
  __ENTRY__(name, request_switch_configuration) = &request_switch_configuration; \
  __ENTRY__(name, request_switch_alternate_setting) = &request_switch_alternate_setting; \
  __ENTRY__(name, support_bulk) = &support_bulk; \
  __ENTRY__(name, support_isochronous) = &support_isochronous; \
  __ENTRY__(name, support_control) = &support_control; \
  __ENTRY__(name, support_interrupt) = &support_interrupt; \
  __ENTRY__(name, support_transfer_type) = &support_transfer_type; \
  __ENTRY__(name, dump_device_desc) = &dump_device_desc; \
  __ENTRY__(name, dump_configuration) = &dump_configuration; \
  __ENTRY__(name, dump_interface) = &dump_interface; \
  __ENTRY__(name, dump_endpoints) = &dump_endpoints; \
  __ENTRY__(name, dump_endpoint) = &dump_endpoint; \
  __ENTRY__(name, build_string) = &build_string; \
  __ENTRY__(name, process_lang_ids) = &process_lang_ids; \
  __ENTRY__(name, dump_device) = &dump_device; \
  __ENTRY__(name, get_free_device_request) = &get_free_device_request; \
  __ENTRY__(name, free_device_request) = &free_device_request; \
  __ENTRY__(name, set_report) = &set_report; \
  __ENTRY__(name, set_protocol) = &set_protocol; \
  __ENTRY__(name, set_idle) = &set_idle; \
  __ENTRY__(name, process_device_descriptor) = &process_device_descriptor; \
  __ENTRY__(name, process_configuration_descriptor) = &process_configuration_descriptor; \
  __ENTRY__(name, process_string_descriptor) = &process_string_descriptor; \
  __ENTRY__(name, set_address) = &set_address; \
  __ENTRY__(name, set_configuration) = &set_configuration; \
  __ENTRY__(name, process_whole_configuration) = &process_whole_configuration; \
  __ENTRY__(name, handle_interface) = &handle_interface; \
  __ENTRY__(name, handle_configuration) = &handle_configuration; \
  __ENTRY__(name, handle_lang) = &handle_lang; \
  __ENTRY__(name, handle_dev) = &handle_dev; \
  __ENTRY__(name, get_max_logic_unit_numbers) = &get_max_logic_unit_numbers; \
  __ENTRY__(name, reset_bulk_only) = &reset_bulk_only; \
  __ENTRY__(name, get_descriptor) = &get_descriptor; \
  __ENTRY__(name, get_req_status) = &get_req_status; \
  __ENTRY__(name, set_feature) = &set_feature; \
  __ENTRY__(name, clear_feature) = &clear_feature; \
  __ENTRY__(name, add_downstream_device) = &add_downstream_device; \
  __ENTRY__(name, add_downstream) = &add_downstream; \
  __ENTRY__(name, delete_usb_dev) = &delete_usb_dev; \
  __ENTRY__(name, free_usb_dev_endpoints) = &free_usb_dev_endpoints; \
  __ENTRY__(name, free_usb_dev_strings) = &free_usb_dev_strings; \
  __ENTRY__(name, free_usb_dev_configs) = &free_usb_dev_configs; \
  __ENTRY__(name, free_usb_dev_interfaces) = &free_usb_dev_interfaces; \
  __ENTRY__(name, get_device_descriptor) = &get_device_descriptor; \
  __ENTRY__(name, get_active_configuration) = &get_active_configuration; \
  __ENTRY__(name, string_build_routine) = &string_build_routine; \
  __ENTRY__(name, interface_build_routine) = &interface_build_routine; \
  __ENTRY__(name, endpoint_build_routine) = &endpoint_build_routine; \
  __ENTRY__(name, __transition_to_address_state) = &__transition_to_address_state; \
  __ENTRY__(name, __transition_to_config_state) = &__transition_to_config_state; \
  __ENTRY__(name, __set_max_packet) = &__set_max_packet; \
  __ENTRY__(name, __build_interface) = &__build_interface; \
  __ENTRY__(name, __set_config_as_active) = &__set_config_as_active; \
  __ENTRY__(name, __retrieve_length_from_config) = &__retrieve_length_from_config; \
  __ENTRY__(name, __set_start) = &__set_start; \
  __ENTRY__(name, __set_end) = &__set_end; \
  __ENTRY__(name, __is_interface) = &__is_interface; \
  __ENTRY__(name, __is_hid) = &__is_hid; \
  __ENTRY__(name, __is_endpoint) = &__is_endpoint; \
  __ENTRY__(name, __request_dev_descriptor) = &__request_dev_descriptor; \
  __ENTRY__(name, __request_config_descriptor) = &__request_config_descriptor; \
  __ENTRY__(name, __request_whole_config) = &__request_whole_config; \
  __ENTRY__(name, __request_set_address) = &__request_set_address; \
  __ENTRY__(name, __request_set_config) = &__request_set_config; \
  __ENTRY__(name, __request_string_descriptor) = &__request_string_descriptor; \
  __ENTRY__(name, __request_set_protocol) = &__request_set_protocol; \
  __ENTRY__(name, __request_set_idle) = &__request_set_idle; \
  __ENTRY__(name, __request_set_report) = &__request_set_report; \
  __ENTRY__(name, __request_reset_bulk_only) = &__request_reset_bulk_only; \
  __ENTRY__(name, __request_get_max_logic_unit_number) = &__request_get_max_logic_unit_number; \
  __ENTRY__(name, __request_get_descriptor) = &__request_get_descriptor; \
  __ENTRY__(name, __request_get_req_status) = &__request_get_req_status; \
  __ENTRY__(name, __request_set_feature) = &__request_set_feature; \
  __ENTRY__(name, __request_clear_feature) = &__request_clear_feature; \
  __ENTRY__(name, __request_switch_alt_setting) = &__request_switch_alt_setting; \
  __ENTRY__(name, __request_switch_config) = &__request_switch_config; \
  __ENTRY__(name, __transfer_type) = &__transfer_type; \
  __ENTRY__(name, __is_transfer_type) = &__is_transfer_type; \
  __ENTRY__(name, __is_interrupt_type) = &__is_interrupt_type; \
  __ENTRY__(name, __is_bulk_type) = &__is_bulk_type; \
  __ENTRY__(name, __is_control_type) = &__is_control_type; \
  __ENTRY__(name, __max_payload) = &__max_payload; \
  __ENTRY__(name, __endpoint_number) = &__endpoint_number; \
  __ENTRY__(name, __endpoint_default_or_not) = &__endpoint_default_or_not; \
  __ENTRY__(name, __direction) = &__direction; \
  __ENTRY__(name, __is_direction) = &__is_direction; \
  __ENTRY__(name, __is_direction_in) = &__is_direction_in; \
  __ENTRY__(name, __is_direction_out) = &__is_direction_out; \
  __ENTRY__(name, __is_x_to_y) = &__is_x_to_y; \
  __ENTRY__(name, __is_device_to_host) = &__is_device_to_host; \
  __ENTRY__(name, __is_host_to_device) = &__is_host_to_device; \
  __ENTRY__(name, __get_alternate_settings) = &__get_alternate_settings; \
  __ENTRY__(name, __has_endpoints) = &__has_endpoints; \
  __ENTRY__(name, __get_first_endpoint) = &__get_first_endpoint; \
  __ENTRY__(name, __match_endpoint) = &__match_endpoint; \
  __ENTRY__(name, __get_endpoint) = &__get_endpoint

#define __STATE_BODY__(dev, cmp, s) \
  dev->state cmp s

#define __CONFIG_STATE__(dev) \
  __STATE_BODY__(dev, __EQUAL__, CONFIGURED_STATE)

#define __NOT_CONFIG_STATE__(dev) \
  __STATE_BODY__(dev, __NOT_EQUAL__, CONFIGURED_STATE)

#define __INITIAL_STATE_BODY__(flags, cmp, state) \
  flags cmp state

#define __BULK_INITIAL__(flags) \
  __INITIAL_STATE_BODY__(flags, __EQUAL__, BULK_INITIAL_STATE)

#define __CTL_INITIAL__(flags) \
  __INITIAL_STATE_BODY__(flags, __EQUAL__, CONTROL_INITIAL_STATE)

#define __BULK_NOT_INITIAL__(flags) \
  __INITIAL_STATE_BODY__(flags, __NOT_EQUAL__, BULK_INITIAL_STATE)

#define __CTL_NOT_INITIAL__(flags) \
  __INITIAL_STATE_BODY__(flags, __NOT_EQUAL__, CONTROL_INITIAL_STATE)

#define __DEV_LOG_ERROR__(dev, message, ...) \
  __STRUCT_CALL__(dev->device_logger, error_c, message, ## __VA_ARGS__)

#define __PROCESS_ROUTINE__(dev, function_ptr, message, ...) \
  __IF_SINGLE_RET__(__IS_NEG_ONE__(__STRUCT_CALL__(dev, function_ptr,  ## __VA_ARGS__)), \
    __DEV_LOG_ERROR__(dev, message))

#define __STRING_ROUTINE__(dev, str, index, string_buffer, name) \
  __IF_NOT_ZERO__(index) { \
    name = string_build_routine(dev, index, string_buffer, str); \
    __IF_RET_NEG__(__IS_NULL__(name)); \
  }

#define __REQUEST_ROUTINE__(dev, request_function, err_message, ...) \
  UsbDeviceRequest *device_req = __STRUCT_CALL__(dev, get_free_device_request); \
  __IF_RET_NEG__(__IS_NULL__(device_req)); \
  request_function(dev, device_req, ## __VA_ARGS__); \
  __IF_COND__(dev->error_while_transfering) { \
    __DEV_LOG_ERROR__(dev, err_message); \
    return __RET_E__; \
  } \
  return __RET_S__;

#define __REQUEST_ROUTINE_CALLBACK__(dev, request_function, ...) \
  UsbDeviceRequest *device_req = __STRUCT_CALL__(dev, get_free_device_request); \
  __IF_RET_NEG__(__IS_NULL__(device_req)); \
  request_function(dev, device_req, ## __VA_ARGS__); \
  return __RET_S__;

#define __USB_DEV_ROUTINE__(dev, interface, data, call, routine_function, pipe, ...) \
  __IF_SINGLE_RET__(!__STRUCT_CALL__(dev, contain_interface, interface), \
    call(dev, interface, E_INTERFACE_NOT_SUPPORTED, data));  \
  unsigned int endpoint_count = \
      interface->active_interface->alternate_interface_desc.bNumEndpoints; \
  Endpoint **endpoints = interface->active_interface->endpoints; \
  __FOR_RANGE__(i, int, 0, endpoint_count) { \
    __IF_CUSTOM__(__STRUCT_CALL__(dev, is_pipe_buildable, endpoints[i], \
      pipe), return routine_function(dev, ## __VA_ARGS__)) \
  } \
  call(dev, interface, E_ENDPOINT_INV, data);

#define __USB_DEV_BULK_ROUTINE__(dev, interface, data, call, pipe, ...) \
  __USB_DEV_ROUTINE__(dev, interface, data, call, \
  ((UsbController *)dev->controller)->bulk_entry_point, pipe, \
  ## __VA_ARGS__)

#define __USB_DEV_INTR_ROUTINE__(dev, interface, data, call, pipe, ...) \
  __USB_DEV_ROUTINE__(dev, interface, data, call, \
  ((UsbController *)dev->controller)->interrupt_entry_point, pipe, \
  ## __VA_ARGS__)

#define __USB_DEV_CTL_ROUTINE__(dev, interface, data, call, pipe, ...) \
  __USB_DEV_ROUTINE__(dev, interface, data, call, \
  dev->request, pipe, ## __VA_ARGS__)

#define __USB_DEV_ISO_ROUTINE__(dev, interface, data, call, pipe, ...) \
  __USB_DEV_ROUTINE__(dev, interface, data, call, \
  ((UsbController *)dev->controller)->iso_entry_point, pipe, ## __VA_ARGS__)

#define __CALLB_PROTO__ void (*callback)(struct UsbDev* dev, \
  Interface* itf, uint32_t status, void* data)

struct UsbDev {
  void (*new_usb_device)(struct UsbDev *dev, uint8_t speed, uint8_t port, uint8_t level,
                    uint8_t removable, uint8_t root_port, uint8_t dev_num, 
                    void *controller, uint8_t supress_flag);
  void (*request_build)(struct UsbDev *dev, UsbDeviceRequest *req,
                        int8_t rq_type, int8_t rq, int16_t value_high,
                        int16_t value_low, int16_t shift, int16_t index,
                        int16_t len);
  uint32_t (*request)(struct UsbDev *dev, struct UsbDeviceRequest *device_request,
                  void *data, uint8_t priority, Interface* interface, Endpoint *endpoint,
                  __CALLB_PROTO__, uint8_t flags);
  uint32_t (*usb_dev_control)(struct UsbDev *dev, Interface *interface,
                          unsigned int pipe, uint8_t priority, void *data,
                          uint8_t *setup, __CALLB_PROTO__, uint8_t flags);
  uint32_t (*usb_dev_interrupt)(struct UsbDev *dev, Interface *interface,
                            unsigned int pipe, uint8_t priority, void *data,
                            unsigned int len, uint16_t interval,
                            __CALLB_PROTO__);
  uint32_t (*usb_dev_bulk)(struct UsbDev *dev, Interface *interface,
                       unsigned int pipe, uint8_t priority, void *data,
                       unsigned int len,
                       __CALLB_PROTO__, uint8_t flags);
  uint32_t (*usb_dev_iso)(struct UsbDev* dev, Interface* interface, unsigned int pipe,
    uint8_t priority, void* data, unsigned int len, uint16_t interval, __CALLB_PROTO__);
  int (*usb_dev_interface_lock)(struct UsbDev *dev, Interface *interface, void* driver);
  void (*usb_dev_free_interface)(struct UsbDev *dev, Interface *interface);
  void (*request_callback)(struct UsbDev *dev, Interface* itf, uint32_t status, void *data);
  int (*is_pipe_buildable)(struct UsbDev* dev, Endpoint *endpoint, unsigned int pipe);
  int (*contain_interface)(struct UsbDev *dev, Interface *interface);
  int (*request_switch_configuration)(
      struct UsbDev *dev, int configuration,
      __CALLB_PROTO__);
  int (*request_switch_alternate_setting)(
      struct UsbDev *dev, Interface *interface, int setting);
  int8_t (*support_bulk)(struct UsbDev *dev, Interface *interface);
  int8_t (*support_isochronous)(struct UsbDev *dev, Interface *interface);
  int8_t (*support_control)(struct UsbDev *dev, Interface *interface);
  int8_t (*support_interrupt)(struct UsbDev *dev, Interface *interface);
  int8_t (*support_transfer_type)(struct UsbDev* dev, Interface* interface, uint8_t transfer_type);
  void (*dump_device_desc)(struct UsbDev *dev);
  void (*dump_configuration)(struct UsbDev *dev);
  void (*dump_interface)(struct UsbDev *dev, Interface *interface);
  void (*dump_endpoints)(struct UsbDev *dev, Interface *interface);
  void (*dump_endpoint)(struct UsbDev *dev, Endpoint *endpoint);
  void (*init_device_functions)(struct UsbDev *dev);
  void (*init_device_logger)(struct UsbDev *dev);
  char *(*build_string)(struct UsbDev *dev, int len, uint8_t *string_buffer);
  void (*process_lang_ids)(struct UsbDev *dev, uint8_t *string_buffer,
                           int s_len);
  void (*dump_device)(struct UsbDev *dev);
  int (*get_max_logic_unit_numbers)(struct UsbDev* dev, Interface* interface, uint8_t* data, __CALLB_PROTO__);
  int (*reset_bulk_only)(struct UsbDev* dev, Interface* interface, __CALLB_PROTO__);
  int (*get_descriptor)(struct UsbDev* dev, Interface* interface, uint8_t* data, unsigned int len,
                      __CALLB_PROTO__);
  int (*get_req_status)(struct UsbDev* dev, Interface* interface, uint8_t* data, unsigned int len, __CALLB_PROTO__);
  int (*set_feature)(struct UsbDev* dev, Interface* interface, uint16_t feature_value, uint16_t port, 
                __CALLB_PROTO__);
  int (*clear_feature)(struct UsbDev* dev, Interface* interface, uint16_t feature_value, uint16_t port,
                 __CALLB_PROTO__);
  UsbDeviceRequest *(*get_free_device_request)(struct UsbDev *dev);
  void (*free_device_request)(struct UsbDev *dev,
                              UsbDeviceRequest *device_request);
  int (*set_report)(struct UsbDev *dev, Interface *interface, uint8_t type,
                    void *data, unsigned int len,
                    __CALLB_PROTO__);
  int (*set_protocol)(struct UsbDev *dev, Interface *interface,
                      uint16_t protocol_value,
                      __CALLB_PROTO__);
  int (*set_idle)(struct UsbDev *dev, Interface *interface);
  int (*set_address)(struct UsbDev *dev, uint8_t address);
  int (*process_device_descriptor)(struct UsbDev *dev,
                                   DeviceDescriptor *device_descriptor,
                                   unsigned int len, uint8_t supress_flag);
  int (*process_configuration_descriptor)(
      struct UsbDev *dev, ConfigurationDescriptor *config_descriptor,
      uint16_t configuration_index, unsigned int len);
  int (*process_string_descriptor)(struct UsbDev *dev, uint8_t *string_buffer,
                                     uint16_t index, uint16_t lang_id, char *s,
                                     unsigned int len);
  int (*set_configuration)(struct UsbDev *dev, uint8_t configuration);
  int (*process_whole_configuration)(struct UsbDev *dev,
                                     uint8_t *configuration_buffer,
                                     uint16_t configuration_index,
                                     unsigned int len);
  int (*handle_interface)(struct UsbDev* dev, Configuration* configuration, uint8_t* string_buffer, 
                      uint8_t* start, uint8_t* end, uint8_t num_interfaces);
  int (*handle_configuration)(struct UsbDev* dev, uint8_t* string_buffer, uint8_t* config_buffer, 
                    ConfigurationDescriptor* config_descriptor, uint8_t num_configurations);
  int (*handle_lang)(struct UsbDev* dev, uint8_t* string_buffer);
  int (*handle_dev)(struct UsbDev* dev, uint8_t* string_buffer, DeviceDescriptor* device_descriptor);
  void (*add_downstream_device)(struct UsbDev* dev, struct UsbDev* downstream_dev);
  void (*add_downstream)(struct UsbDev* dev, uint8_t downstream_ports);
  void (*delete_usb_dev)(struct UsbDev* dev);
  void (*free_usb_dev_strings)(struct UsbDev* dev);
  void (*free_usb_dev_configs)(struct UsbDev* dev);
  void (*free_usb_dev_interfaces)(struct UsbDev* dev, Interface** interfaces, int num_interfaces);
  void (*free_usb_dev_endpoints)(struct UsbDev* dev, Endpoint** endpoints, int num_endpoints);
  char* (*string_build_routine)(struct UsbDev* dev, unsigned int string_index,
    uint8_t* string_buffer, char* string_repr);
  int (*interface_build_routine)(struct UsbDev* dev, MemoryService_C* mem_service,
    uint8_t* start, uint8_t* string_buffer, Interface** interfaces,
    Alternate_Interface** prev, unsigned int* prev_interface_number,
    unsigned int* interface_num);
  void (*endpoint_build_routine)(struct UsbDev* dev, MemoryService_C* mem_service,
    unsigned int* endpoint_num, Alternate_Interface* prev, uint8_t* start);
  __DECLARE_STRUCT_GET__(device_descriptor, struct UsbDev*, DeviceDescriptor);
  __DECLARE_STRUCT_GET__(active_configuration, struct UsbDev*, Configuration*);
  void (*__transition_to_address_state)(struct UsbDev* dev);
  void (*__transition_to_config_state)(struct UsbDev* dev);
  void (*__set_max_packet)(struct UsbDev* dev, DeviceDescriptor* dev_desc);
  Interface* (*__build_interface)(struct UsbDev* dev, Alternate_Interface* alt_interface,
    char* interface_description, MemoryService_C* m);
  void (*__set_config_as_active)(struct UsbDev* dev, Configuration* configuration, 
    uint8_t* sel);
  void (*__retrieve_length_from_config)(struct UsbDev* dev, 
    uint16_t* total_len, uint8_t* desc_len, ConfigurationDescriptor* config_descriptor);
  void (*__set_start)(struct UsbDev* dev, uint8_t* config_buffer, 
    uint8_t* desc_len, uint8_t** start);
  void (*__set_end)(struct UsbDev* dev, uint8_t* config_buffer, 
    uint16_t* tot_len, uint8_t** end);
  uint8_t (*__is_interface)(struct UsbDev* dev, uint8_t* start);
  uint8_t (*__is_hid)(struct UsbDev* dev, uint8_t* start);
  uint8_t (*__is_endpoint)(struct UsbDev* dev, uint8_t* start);
  void (*__request_dev_descriptor)(struct UsbDev* dev, 
    UsbDeviceRequest* device_req, DeviceDescriptor* device_descriptor, 
    unsigned int len, uint8_t flag);
  void (*__request_config_descriptor)(struct UsbDev* dev, 
    UsbDeviceRequest* device_req, ConfigurationDescriptor* config_descriptor, 
    uint16_t configuration_index, unsigned int len);
  void (*__request_whole_config)(struct UsbDev* dev, UsbDeviceRequest* device_req, 
    uint8_t* config_buffer, uint16_t configuration_index, unsigned int len);
  void (*__request_set_address)(struct UsbDev* dev, 
    UsbDeviceRequest* device_req, uint8_t addr);
  void (*__request_set_config)(struct UsbDev* dev, UsbDeviceRequest* device_req);
  void (*__request_string_descriptor)(struct UsbDev* dev, 
    UsbDeviceRequest* device_req, uint8_t* string_buffer, 
    uint16_t index, uint16_t lang_id, unsigned int len);
  void (*__request_set_protocol)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    Interface* interface, __CALLB_PROTO__);
  void (*__request_set_idle)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    Interface* interface);
  void (*__request_set_report)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    Interface* interface, uint8_t type, unsigned int len, void* data, 
    __CALLB_PROTO__);
  void (*__request_reset_bulk_only)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    Interface* interface, __CALLB_PROTO__);
  void (*__request_get_max_logic_unit_number)(struct UsbDev* dev, 
    UsbDeviceRequest* device_req, Interface* interface, void* data,
    __CALLB_PROTO__);
  void (*__request_get_descriptor)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    void* data, unsigned int len, __CALLB_PROTO__);
  void (*__request_get_req_status)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    void* data, unsigned int len, __CALLB_PROTO__);
  void (*__request_set_feature)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    uint16_t feature_value, uint16_t port, __CALLB_PROTO__);
  void (*__request_clear_feature)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    uint16_t feature_value, uint16_t port, __CALLB_PROTO__);
  void (*__request_switch_alt_setting)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    Interface* interface, Alternate_Interface* alt_itf);
  void (*__request_switch_config)(struct UsbDev* dev, UsbDeviceRequest* device_req,
    int configuration, __CALLB_PROTO__);
  uint8_t (*__transfer_type)(struct UsbDev* dev, Endpoint* e);
  int (*__is_transfer_type)(struct UsbDev* dev, Endpoint* e, uint8_t transfer_type);
  int (*__is_interrupt_type)(struct UsbDev* dev, Endpoint* e);
  int (*__is_bulk_type)(struct UsbDev* dev, Endpoint* e);
  int (*__is_control_type)(struct UsbDev* dev, Endpoint* e);
  uint16_t (*__max_payload)(struct UsbDev* dev, Endpoint* e);
  uint8_t (*__endpoint_number)(struct UsbDev* dev, Endpoint* e);
  uint8_t (*__endpoint_default_or_not)(struct UsbDev* dev, Endpoint* e);
  uint8_t (*__direction)(struct UsbDev* dev, Endpoint* e);
  uint8_t (*__is_direction)(struct UsbDev* dev, Endpoint* e, uint8_t direction);
  uint8_t (*__is_direction_in)(struct UsbDev* dev, Endpoint* e);
  uint8_t (*__is_direction_out)(struct UsbDev* dev, Endpoint* e);
  uint8_t (*__is_x_to_y)(struct UsbDev* dev, UsbDeviceRequest* dev_req,
    uint8_t x_to_y);
  uint8_t (*__is_device_to_host)(struct UsbDev* dev, UsbDeviceRequest* dev_req);
  uint8_t (*__is_host_to_device)(struct UsbDev* dev, UsbDeviceRequest* dev_req);
  uint8_t (*__get_alternate_settings)(struct UsbDev* dev, Interface* interface);
  int8_t (*__has_endpoints)(struct UsbDev* dev, Alternate_Interface* alt_itf);
  Endpoint* (*__get_first_endpoint)(struct UsbDev* dev, Alternate_Interface* alt_itf);
  int8_t (*__match_endpoint)(struct UsbDev* dev, Endpoint* e, uint8_t endpoint_number);
  Endpoint* (*__get_endpoint)(struct UsbDev* dev, Alternate_Interface* alt_interface,
    uint8_t endpoint_number);

  uint8_t speed;
  uint8_t port;
  uint8_t address;
  uint16_t max_packet_size; // for endpoint 0
  struct DeviceDescriptor device_desc;
  // current active in dev
  struct Configuration *active_config;
  // supported configuration on device
  // used for displaying all supported configuration x interfaces on device
  struct Configuration **supported_configs;
  uint8_t state; // default , address, configured -> helpful when doing control
                 // transfer for polling (!configured) !!!
  Logger_C *device_logger;
  uint8_t error_while_transfering; // only used when not configured -> helpful
                                   // to pass the error
  void *controller; // recovery of uhci in entry point methods
  uint16_t *lang_ids;
  uint16_t lang_id;
  char *manufacturer;
  char *product;
  char *serial_number;
  uint8_t *device_request_map_io;
  uint8_t device_request_map_io_bitmap[PAGE_SIZE / sizeof(UsbDeviceRequest)];
  list_element l_e;        // controller element linkage
  list_element l_e_driver; // driver element linkage
  Mutex_C* device_mutex;
  struct UsbDev** downstream_devs;
  uint8_t downstream_count;
  uint8_t max_down_stream;
  uint8_t level;
  uint8_t rootport;
  uint8_t dev_num;
  uint8_t removable;
};

typedef struct UsbDev UsbDev;

extern MemoryService_C* __DEV_MEMORY(UsbDev* dev);

typedef void (*callback_function)(UsbDev* dev, Interface* itf, uint32_t status, void *data);

void new_usb_device(UsbDev *dev, uint8_t speed, uint8_t port, uint8_t level,
                    uint8_t removable, uint8_t root_port, uint8_t dev_num, 
                    void *controller, uint8_t supress_flag);

#endif