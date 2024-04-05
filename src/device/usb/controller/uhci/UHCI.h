#ifndef UHCI__INCLUDE
#define UHCI__INCLUDE

#include "../UsbControllerFlags.h"
#include "data/UHCI_Data.h"
#include "components/UHCIMemory.h"
#include "components/UHCIMemory.h"
#include "../UsbController.h"
#include "../../events/EventDispatcher.h"
#include "../../interfaces/LoggerInterface.h"
#include "../../interfaces/MapInterface.h"
#include "../../interfaces/MutexInterface.h"
#include "../../interfaces/PciDeviceInterface.h"
#include "../../interfaces/SystemInterface.h"
#include "stdint.h"
#include "../../utility/Utils.h"
#include "../../dev/UsbDevice.h"
#include "../../dev/data/UsbDev_Data.h"

// all debugs turned on
//#define DEBUG_ON
// only specfic debugs
//#define TD_DEBUG_ON //-> inspect TD
//#define QH_DEBUG_ON //-> inspect QH
//#define TRANSFER_DEBUG_ON //-> inspect whole TRANSFER
//#define DEVICE_DEBUG_ON //-> inspect dev
//#define SKELETON_DEBUG_ON //-> inspect skeleton when creating
//#define REGISTER_DEBUG_ON //-> inspect register
//#define STATUS_DEBUG_ON //-> inspect status codes
//#define PCI_DEBUG_ON //-> inspect PCI Room
//#define TRANSFER_MEASURE_ON //-> measures the time for a transfer
//#define MEASURE_MS // -> measures in millis
//#define MEASURE_NS // -> measures in nano
//#define MEASURE_MCS // -> measures in micro

// time delay
#define USB_TDRSTR 50
#define USB_TDRST 10
#define USB_TRHRSI 3
#define USB_TRSTRCY 10

#define UPPER_BOUND_TIME_OUT_MILLIS_CONTROL 10
#define UPPER_BOUND_TIME_OUT_MILLIS_BULK 50

extern const uint8_t CLASS_ID;
extern const uint8_t SUBCLASS_ID;
extern const uint8_t INTERFACE_ID;

struct _UHCI {
  struct UsbController super;
  void (*new_UHCI)(struct _UHCI *uhci, PciDevice_Struct* pci_device, SystemService_C* mem_service);

  Register *(*look_for)(struct _UHCI *uhci, Register_Type r);
  // int8_t (*write)(struct _UHCI* uhci, Register* reg, void* b);
  // int8_t (*read)(struct _UHCI* uhci, Register* reg);

  // void* (*get_reg_data)(struct _UHCI* uhci, Register* reg);

  // char* (*get_information)(struct _UHCI* uhci);

  QH* (*request_frames)(struct _UHCI* uhci);
  Addr_Region* (*i_o_space_layout_run)(struct _UHCI* uhci);
  Register** (*request_register)(struct _UHCI* uhci);

  void (*insert_queue)(struct _UHCI *uhci, struct QH *new_qh,
                       uint16_t priority, enum QH_HEADS v);
  unsigned int (*retransmission)(struct _UHCI *uhci, struct QH *process_qh);
  uint32_t (*get_status)(struct _UHCI *uhci, struct TD *td);
  void (*remove_queue)(struct _UHCI *uhci, struct QH *qh);
  uint32_t (*wait_poll)(struct _UHCI *uhci, struct QH *process_qh, uint32_t timeout);
  void (*traverse_skeleton)(struct _UHCI *uhci, struct QH *entry);

  UsbPacket *(*create_USB_Packet)(struct _UHCI *uhci, UsbDev *dev,
                                  UsbPacket *prev, struct TokenValues token,
                                  int8_t speed, void *data, int last_packet, uint8_t flags);

  void (*dump_all)(struct _UHCI *uhci);
  void (*dump_skeleton)(struct _UHCI *uhci);
  void (*inspect_TD)(struct _UHCI *uhci, struct TD *td);
  void (*inspect_QH)(struct _UHCI *uhci, struct QH *qh);
  void (*inspect_transfer)(struct _UHCI *uhci, struct QH *qh, struct TD *td);

  void (*print_USB_Transaction)(struct _UHCI *uhci, UsbTransaction *transaction,
                                int order);
  void (*print_USB_Transfer)(struct _UHCI *uhci, UsbTransfer *transfer);

  QH *(*get_free_qh)(struct _UHCI *uhci);
  TD *(*get_free_td)(struct _UHCI *uhci);

  void (*free_qh)(struct _UHCI *uhci, QH *qh);
  void (*free_td)(struct _UHCI *uhci, TD *td);

  void (*control_transfer)(struct _UHCI *uhci, UsbDev *dev,
                           struct UsbDeviceRequest *rq, void *data,
                           uint8_t priority, Endpoint *endpoint,
                           UsbTransfer *(*build_control_transfer)(
                               struct _UHCI *uhci, UsbDev *dev,
                               struct UsbDeviceRequest *device_request,
                               void *data, Endpoint *endpoint, uint8_t flags),
                           callback_function callback, uint8_t flags);
  void (*interrupt_transfer)(struct _UHCI *uhci, UsbDev *dev, void *data,
                             unsigned int len, uint16_t interval,
                             uint8_t priority, Endpoint *e,
                             UsbTransfer *(*build_bulk_or_interrupt_transfer)(
                                 struct _UHCI *uhci, UsbDev *dev, void *data,
                                 Endpoint *e, unsigned int len,
                                 const char *type, uint8_t flags),
                             callback_function callback);
  void (*bulk_transfer)(struct _UHCI *uhci, UsbDev *dev, void *data,
                        unsigned int len, uint8_t priority, Endpoint *e,
                        UsbTransfer *(*build_bulk_or_interrupt_transfer)(
                            struct _UHCI *uhci, UsbDev *dev, void *data,
                            Endpoint *e, unsigned int len, const char *type, uint8_t flags),
                        callback_function callback, uint8_t flags);
  int16_t (*is_valid_priority)(struct _UHCI *uhci, UsbDev *dev,
                               uint8_t priority, callback_function callback);
  void (*init_controller_functions)(struct _UHCI *uhci);
  Logger_C* (*init_logger)(struct _UHCI *uhci, MemoryService_C *m);
  Mutex_C* (*init_mutex)(struct _UHCI* uhci, MemoryService_C* m);
  void (*init_maps)(struct _UHCI *uhci, MemoryService_C *m);
  void (*fill_maps)(struct _UHCI *uhci);

  void (*create_dev)(struct _UHCI *uhci, int16_t status, int pn,
                     MemoryService_C *m);

  void (*remove_transfer_entry)(struct _UHCI *uhci, QH *entry);


  void (*remove_td_linkage)(struct _UHCI *uhci, TD *start);

  int (*supported_event_listener_type)(struct _UHCI *uhci,
                                       uint16_t event_listener_type);

  void (*dump_uhci_entry)(struct _UHCI* uhci);                                       
  void (*destroy_transfer)(struct _UHCI* uhci, UsbTransfer* transfer);

  SystemService_C *(*request_interrupt_service)(struct _UHCI* uhci, SystemService_C* mem_service);

  void (*controller_port_configuration)(struct _UHCI* uhci);

  int (*controller_configuration)(struct _UHCI* uhci);

  int16_t (*is_valid_interval)(struct _UHCI* uhci, UsbDev* dev, uint16_t interval, void* data);

  Register **i_o_registers;
  QH *qh_entry;
  Addr_Region *addr_region;
  PciDevice_Struct *pci_device;
  SystemService_C *mem_service;
  SystemService_C *interrupt_service;
  uint8_t irq;
  uint32_t fba;

  Mutex_C *mutex;

  Logger_C *controller_logger;

  SuperMap *register_look_up;
  SuperMap *qh_to_td_map;
  SuperMap *callback_map;
  SuperMap *qh_data_map;
  SuperMap *qh_dev_map;
  SuperMap *qh_device_request_map;

  #if defined(TRANSFER_MEASURE_ON)
  SuperMap * qh_measurement;
  #endif

  uint8_t *map_io_buffer_qh; // 1 page
  uint8_t *map_io_buffer_td; // 2 page

  uint8_t map_io_buffer_bit_map_qh[PAGE_SIZE / sizeof(QH)];
  uint8_t map_io_buffer_bit_map_td[(2 * PAGE_SIZE) / sizeof(TD)];

  uint8_t signal;
  uint8_t signal_not_override; // possibility that we just passed the QH* that got transfered,
  // meaning by setting this value, we will just continue to traverse the whole skeleton
};

typedef struct _UHCI _UHCI;

void new_UHCI(struct _UHCI *uhci, PciDevice_Struct* pci_device, SystemService_C *mem_service);

QH* request_frames(struct _UHCI* uhci);
Addr_Region *i_o_space_layout_run(struct _UHCI* uhci);
Register** request_register(struct _UHCI* uhci);

void bulk_entry_point_uhci(struct UsbDev *dev, Endpoint *endpoint, void *data,
                      unsigned int len, uint8_t priority,
                      callback_function callback, uint8_t flags);
void control_entry_point_uhci(struct UsbDev *dev,
                         struct UsbDeviceRequest *device_request, void *data,
                         uint8_t priority, Endpoint *endpoint,
                         callback_function callback, uint8_t flags);
void interrupt_entry_point_uhci(struct UsbDev *dev, Endpoint *endpoint, void *data,
                           unsigned int len, uint8_t priority,
                           uint16_t interval, callback_function callback);

void insert_queue(struct _UHCI *uhci, struct QH *new_qh,
                  uint16_t priority, enum QH_HEADS v);
void remove_queue(struct _UHCI *uhci, struct QH *qh);

SystemService_C *request_interrupt_service(_UHCI* uhci, SystemService_C* mem_service);

typedef UsbTransfer *(*build_control_transfer)(
    _UHCI *uhci, UsbDev *dev, struct UsbDeviceRequest *device_request,
    void *data, Endpoint *endpoint, uint8_t flags);
typedef UsbTransfer *(*build_bulk_or_interrupt_transfer)(
    _UHCI *uhci, UsbDev *dev, void *data, Endpoint *e, unsigned int len,
    const char *type, uint8_t flags);

void control_transfer(_UHCI *uhci, UsbDev *dev, struct UsbDeviceRequest *rq,
                      void *data, uint8_t priority, Endpoint *endpoint,
                      build_control_transfer build_function,
                      callback_function callback, uint8_t flags);
void interrupt_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                        uint16_t interval, uint8_t priority, Endpoint *e,
                        build_bulk_or_interrupt_transfer build_function,
                        callback_function callback);
void bulk_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                   uint8_t, Endpoint *e, build_bulk_or_interrupt_transfer build_function,
                   callback_function callback, uint8_t flags);

uint32_t wait_poll(_UHCI *uhci, QH *process_qh, uint32_t timeout);

void _poll_uhci_(UsbController *controller);

QH *get_free_qh(_UHCI *uhci);
TD *get_free_td(_UHCI *uhci);

void free_qh(_UHCI *uhci, QH *qh);
void free_td(_UHCI *uhci, TD *td);

void init_control_transfer(UsbController *controller, Interface *interface,
                           unsigned int pipe, uint8_t priority, void *data,
                           uint8_t *setup, callback_function callback);
void init_interrupt_transfer(UsbController *controller, Interface *interface,
                             unsigned int pipe, uint8_t priority, void *data,
                             unsigned int len, uint16_t interval,
                             callback_function callback);
void init_bulk_transfer(UsbController *controller, Interface *interface,
                        unsigned int pipe, uint8_t priority, void *data,
                        unsigned len, callback_function callback);

unsigned int retransmission(_UHCI *uhci, struct QH *process_qh);

void traverse_skeleton(_UHCI *uhci, struct QH *entry);

uint32_t get_status(_UHCI *uhci, TD *td);

int register_driver_uhci(UsbController *controller, struct UsbDriver *driver);
int deregister_driver_uhci(UsbController *controller, struct UsbDriver *driver);

void link_device_to_driver_uhci(UsbController* controller, UsbDev *dev, UsbDriver *driver);
void link_driver_to_controller_uhci(UsbController* controller, UsbDriver *driver);
void link_driver_to_interface_uhci(UsbController *controller, UsbDriver *driver,
                              Interface *interface);

UsbPacket *create_USB_Packet(_UHCI *uhci, UsbDev *dev, UsbPacket *prev,
                             struct TokenValues token, int8_t speed, void *data,
                             int last_packet, uint8_t flags);

UsbTransfer *build_control(_UHCI *uhci, UsbDev *dev,
                           UsbDeviceRequest *device_request, void *data,
                           Endpoint *e, uint8_t flags);
UsbTransfer *build_interrupt_or_bulk(_UHCI *uhci, UsbDev *dev, void *data,
                                     Endpoint *e, unsigned int len,
                                     const char *type, uint8_t flags);

int uhci_contain_interface(UsbController *controller, Interface *interface);
int16_t is_valid_priority(_UHCI *uhci, UsbDev *dev, uint8_t priority,
                          callback_function callback);

UsbControllerType is_of_type_uhci(UsbController *controller);

Register *look_for_reg(_UHCI *uhci, Register_Type type);
uint16_t uhci_reset_port(UsbController *controller, uint8_t port);

void init_controller_functions(_UHCI *uhci);
Logger_C* init_logger(_UHCI *uhci, MemoryService_C *m);
Mutex_C* init_mutex(_UHCI* uhci, MemoryService_C* m);
void init_maps(_UHCI *uhci, MemoryService_C *m);
void fill_maps(_UHCI *uhci);

void create_dev(_UHCI *uhci, int16_t status, int pn, MemoryService_C *m);

void remove_transfer_entry(_UHCI *uhci, QH *entry);

void dump_all(_UHCI *uhci);
void dump_skeleton(_UHCI *uhci);
void inspect_TD(_UHCI *uhci, struct TD *td);
void inspect_QH(_UHCI *uhci, struct QH *qh);
void inspect_transfer(_UHCI *uhci, struct QH *qh, struct TD *td);

void print_USB_Transaction(_UHCI *uhci, UsbTransaction *transaction, int order);
void print_USB_Transfer(_UHCI *uhci, UsbTransfer *transfer);

void remove_td_linkage(_UHCI *uhci, TD *start);

void dump_uhci_entry(_UHCI* uhci);

void handler_function_uhci(UsbController* controller);

void runnable_function_uhci(UsbController* controller);

void destroy_transfer(_UHCI* uhci, UsbTransfer* transfer);

void controller_port_configuration(_UHCI* uhci);

int controller_configuration(_UHCI* uhci);

int16_t is_valid_interval(_UHCI* uhci, UsbDev* dev, uint16_t interval, void* data);

#endif