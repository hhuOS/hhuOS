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

struct UsbDev {
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
  // void* driver;

  Logger_C *device_logger;

  uint8_t error_while_transfering; // only used when not configured -> helpful
                                   // to pass the error

  void *controller; // recovery of uhci in entry point methods

  uint16_t *lang_ids;

  uint16_t lang_id;

  char *manufacturer;

  char *product;

  char *serial_number;

  void (*new_usb_device)(struct UsbDev *dev, uint8_t speed, uint8_t port, uint8_t level,
                    uint8_t removable, uint8_t root_port, uint8_t dev_num, SystemService_C *m, 
                    void *controller);

  void (*request_build)(struct UsbDev *dev, UsbDeviceRequest *req,
                        int8_t rq_type, int8_t rq, int16_t value_high,
                        int16_t value_low, int16_t shift, int16_t index,
                        int16_t len);
  void (*request)(struct UsbDev *dev, struct UsbDeviceRequest *device_request,
                  void *data, uint8_t priority, Endpoint *endpoint,
                  void (*callback_function)(struct UsbDev* dev, uint32_t status,
                                            void *data), uint8_t flags);

  void (*usb_dev_control)(struct UsbDev *dev, Interface *interface,
                          unsigned int pipe, uint8_t priority, void *data,
                          uint8_t *setup,
                          void (*callback_function)(struct UsbDev *dev,
                                                    uint32_t status,
                                                    void *data), uint8_t flags);
  void (*usb_dev_interrupt)(struct UsbDev *dev, Interface *interface,
                            unsigned int pipe, uint8_t priority, void *data,
                            unsigned int len, uint16_t interval,
                            void (*callback_function)(struct UsbDev* dev,
                                                      uint32_t status,
                                                      void *data));

  void (*usb_dev_bulk)(struct UsbDev *dev, Interface *interface,
                       unsigned int pipe, uint8_t priority, void *data,
                       unsigned int len,
                       void (*callback_function)(struct UsbDev* dev,
                                                 uint32_t status, void *data), uint8_t flags);

  int (*usb_dev_interface_lock)(struct UsbDev *dev, Interface *interface, void* driver);
  void (*usb_dev_free_interface)(struct UsbDev *dev, Interface *interface);

  void (*request_callback)(struct UsbDev *dev, uint32_t status, void *data);
  int (*is_pipe_buildable)(Endpoint *endpoint, unsigned int pipe);

  int (*contain_interface)(struct UsbDev *dev, Interface *interface);
  int (*request_switch_configuration)(
      struct UsbDev *dev, int configuration,
      void (*callback_function)(struct UsbDev *dev, uint32_t status,
                                void *data));
  int (*request_switch_alternate_setting)(
      struct UsbDev *dev, Interface *interface, int setting,
      void (*callback_function)(struct UsbDev *dev, uint32_t status,
                                void *data));

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

  int (*get_max_logic_unit_numbers)(struct UsbDev* dev, Interface* interface, uint8_t* data, void (*callback_function)(struct UsbDev *dev,
                                              uint32_t status, void *data));
  int (*reset_bulk_only)(struct UsbDev* dev, Interface* interface, void (*callback_function)(struct UsbDev *dev,
                                              uint32_t status, void *data));

  int (*get_descriptor)(struct UsbDev* dev, Interface* interface, uint8_t* data, unsigned int len,
                      void (*callback_function)(struct UsbDev *dev,
                                              uint32_t status, void *data));
  int (*get_req_status)(struct UsbDev* dev, Interface* interface, uint8_t* data, unsigned int len, void (*callback_function)(struct UsbDev *dev,
                                              uint32_t status, void *data));
  int (*set_feature)(struct UsbDev* dev, Interface* interface, uint16_t feature_value, uint16_t port, 
                void (*callback_function)(struct UsbDev *dev,
                                              uint32_t status, void *data));
  int (*clear_feature)(struct UsbDev* dev, Interface* interface, uint16_t feature_value, uint16_t port,
                  void (*callback_function)(struct UsbDev *dev,
                                              uint32_t status, void *data));
  UsbDeviceRequest *(*get_free_device_request)(struct UsbDev *dev);
  void (*free_device_request)(struct UsbDev *dev,
                              UsbDeviceRequest *device_request);
  int (*set_report)(struct UsbDev *dev, Interface *interface, uint8_t type,
                    void *data, unsigned int len,
                    void (*callback_function)(struct UsbDev *dev,
                                              uint32_t status, void *data));

  int (*set_protocol)(struct UsbDev *dev, Interface *interface,
                      uint16_t protocol_value,
                      void (*callback_function)(struct UsbDev *dev,
                                                uint32_t status, void *data));
  int (*set_idle)(struct UsbDev *dev, Interface *interface);
  int (*set_address)(struct UsbDev *dev, uint8_t address);
  int (*process_device_descriptor)(struct UsbDev *dev,
                                   DeviceDescriptor *device_descriptor,
                                   unsigned int len);
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

  uint8_t *device_request_map_io;
  uint8_t device_request_map_io_bitmap[PAGE_SIZE / sizeof(UsbDeviceRequest)];

  list_element l_e;        // controller element linkage
  list_element l_e_driver; // driver element linkage

  Mutex_C* device_mutex;
  SystemService_C *mem_service;

  struct UsbDev** downstream_devs;
  uint8_t downstream_count;

  uint8_t max_down_stream;

  uint8_t level;
  uint8_t rootport;
  uint8_t dev_num;
  uint8_t removable;
};

typedef struct UsbDev UsbDev;

typedef void (*callback_function)(UsbDev* dev, uint32_t status, void *data);

void new_usb_device(UsbDev *dev, uint8_t speed, uint8_t port, uint8_t level,
                    uint8_t removable, uint8_t root_port, uint8_t dev_num, SystemService_C *m, 
                    void *controller);

void add_downstream_device(UsbDev* dev, UsbDev* downstream_dev);
void add_downstream(UsbDev* dev, uint8_t downstream_ports);
void delete_usb_dev(UsbDev* dev);
void free_usb_dev_strings(UsbDev* dev);
void free_usb_dev_configs(UsbDev* dev);
void free_usb_dev_interfaces(UsbDev* dev, Interface** interfaces, int num_interfaces);
void free_usb_dev_endpoints(UsbDev* dev, Endpoint** endpoints, int num_endpoints);

// pipe = endpoint information to create the pipe
void usb_dev_control(UsbDev *dev, Interface *interface, unsigned int pipe,
                     uint8_t priority, void *data, uint8_t *setup,
                     callback_function callback, uint8_t flags);
void usb_dev_interrupt(UsbDev *dev, Interface *interface, unsigned int pipe,
                       uint8_t priority, void *data, unsigned int len,
                       uint16_t interval, callback_function callback);
void usb_dev_bulk(struct UsbDev *dev, Interface *interface, unsigned int pipe,
                  uint8_t priority, void *data, unsigned int len,
                  callback_function callback, uint8_t flags);

int usb_dev_interface_lock(UsbDev *dev, Interface *interface, void* driver);
void usb_dev_free_interface(UsbDev *dev, Interface *interface);

void request_callback(struct UsbDev *dev, uint32_t status, void *data);
void request_build(struct UsbDev *dev, UsbDeviceRequest *req, int8_t rq_type,
                   int8_t rq, int16_t value_high, int16_t value_low,
                   int16_t shift, int16_t index, int16_t len);
void request(struct UsbDev *dev, struct UsbDeviceRequest *device_request,
             void *data, uint8_t priority, Endpoint *endpoint,
             callback_function callback, uint8_t flags);

int8_t support_bulk(struct UsbDev *dev, Interface *interface);
int8_t support_isochronous(struct UsbDev *dev, Interface *interface);
int8_t support_control(struct UsbDev *dev, Interface *interface);
int8_t support_interrupt(struct UsbDev *dev, Interface *interface);

int8_t support_transfer_type(struct UsbDev* dev, Interface* interface, uint8_t transfer_type);

int is_pipe_buildable(Endpoint *endpoint, unsigned int pipe);

int contain_interface(UsbDev *dev, Interface *interface);
int request_switch_configuration(struct UsbDev *dev, int configuration,
                                 callback_function callback);
int request_switch_alternate_setting(struct UsbDev *dev, Interface *interface,
                                     int setting, callback_function callback);

void dump_device_desc(struct UsbDev *dev);
void dump_configuration(struct UsbDev *dev);
void dump_interface(struct UsbDev *dev, Interface *interface);
void dump_endpoints(struct UsbDev *dev, Interface *interface);
void dump_endpoint(struct UsbDev *dev, Endpoint *endpoint);

void init_device_functions(UsbDev *dev);
void init_device_logger(UsbDev *dev);

char *build_string(UsbDev *dev, int len, uint8_t *string_buffer);
void process_lang_ids(UsbDev *dev, uint8_t *string_buffer, int s_len);

void dump_device(struct UsbDev *dev);

int get_max_logic_unit_numbers(UsbDev* dev, Interface* interface, uint8_t* data, callback_function callback);
int reset_bulk_only(UsbDev* dev, Interface* interface, callback_function callback);

UsbDeviceRequest *get_free_device_request(UsbDev *dev);
void free_device_request(UsbDev *dev, UsbDeviceRequest *device_request);
int set_report(UsbDev *dev, Interface *interface, uint8_t type, void *data,
               unsigned int len, callback_function callback);
int set_protocol(UsbDev *dev, Interface *interface, uint16_t protocol_value,
                 callback_function callback);
int set_idle(UsbDev *dev, Interface *interface);
int process_device_descriptor(UsbDev *dev, DeviceDescriptor *device_descriptor,
                              unsigned int len);
int process_configuration_descriptor(UsbDev *dev,
                                     ConfigurationDescriptor *config_descriptor,
                                     uint16_t configuration_index,
                                     unsigned int len);
int process_string_descriptor(UsbDev *dev, uint8_t *string_buffer,
                                uint16_t index, uint16_t lang_id, char *s,
                                unsigned int len);
int set_address(UsbDev *dev, uint8_t address);
int set_configuration(UsbDev *dev, uint8_t configuration);
int process_whole_configuration(UsbDev *dev, uint8_t *configuration_buffer,
                                uint16_t configuration_index, unsigned int len);

int handle_interface(UsbDev* dev, Configuration* configuration, uint8_t* string_buffer, 
                      uint8_t* start, uint8_t* end, uint8_t num_interfaces);
int handle_configuration(UsbDev* dev, uint8_t* string_buffer, uint8_t* config_buffer, 
                ConfigurationDescriptor* config_descriptor, uint8_t num_configurations);
int handle_lang(UsbDev* dev, uint8_t* string_buffer);
int handle_dev(UsbDev* dev, uint8_t* string_buffer, DeviceDescriptor* device_descriptor);

int get_descriptor(UsbDev* dev, Interface* interface, uint8_t* data, unsigned int len, callback_function callback);
int get_req_status(UsbDev* dev, Interface* interface, uint8_t* data, unsigned int len, callback_function callback);
int clear_feature(UsbDev* dev, Interface* interface, uint16_t feature_value, uint16_t port,
                  callback_function callback);
int set_feature(UsbDev* dev, Interface* interface, uint16_t feature_value, uint16_t port, 
                callback_function callback);

#endif