#ifndef UHCI__INCLUDE
#define UHCI__INCLUDE

#include "../UsbControllerFlags.h"
#include "data/UHCI_Data.h"
#include "../components/ControllerMemory.h"
#include "../components/ControllerMemoryTypes.h"
#include "../components/ControllerRegister.h"
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

#define DECLARE_TYPE_UHCI(name, mem_service, ...) \
    MemoryService_C* m = (MemoryService_C*)container_of(mem_service, \
        MemoryService_C, super); \
    name = __ALLOC_KERNEL_MEM__(m, _UHCI, sizeof(_UHCI)); \
    __STRUCT_INIT__(name, new_UHCI, new_UHCI, ## __VA_ARGS__ , \
        mem_service)

#define __QH_DEFAULT__(name) \
    (name)->pyhsicalQHEP = 0; \
    (name)->pyhsicalQHLP = 0; \
    (name)->flags = 0; \
    (name)->parent = 0

#define __QH_ASSIGN__(buffer, offset, name) \
    (name) = (QH*)((buffer) + (offset)); \
    (offset) += sizeof(QH) 

#define __QH_ASSIGN_DEFAULT__(buffer, offset, name) \
    __QH_ASSIGN__((buffer), (offset), (name)); \
    __QH_DEFAULT__((name))

#define __INIT_UHCI__(uhci, m, pci) \
    __SUPER__(uhci, poll)                      = &_poll_uhci_; \
    __SUPER__(uhci, control)                   = &init_control_transfer; \
    __SUPER__(uhci, interrupt)                 = &init_interrupt_transfer; \
    __SUPER__(uhci, bulk)                      = &init_bulk_transfer; \
    __SUPER__(uhci, contains_interface)        = &uhci_contain_interface; \
    __SUPER__(uhci, is_of_type)                = &is_of_type_uhci; \
    __SUPER__(uhci, reset_port)                = &uhci_reset_port; \
    __SUPER__(uhci, interrupt_entry_point)     = &interrupt_entry_point_uhci; \
    __SUPER__(uhci, control_entry_point)       = &control_entry_point_uhci; \
    __SUPER__(uhci, bulk_entry_point)          = &bulk_entry_point_uhci; \
    __SUPER__(uhci, addr_address_region)        = &i_o_space_layout_run; \
    __SUPER__(uhci, new_usb_controller)        = &new_super_usb_controller; \
    __SUPER__(uhci, add_registers)             = &request_register; \
    __SUPER__(uhci, handler_function)          = &handler_function_uhci; \
    __SUPER__(uhci, runnable_function)         = &runnable_function_uhci; \
    \
    __ENTRY__(uhci, dump_uhci_entry)               = &dump_uhci_entry; \
    __ENTRY__(uhci, request_frames)                = &request_frames; \
    __ENTRY__(uhci, insert_queue)                  = &insert_queue; \
    __ENTRY__(uhci, retransmission)                = &retransmission; \
    __ENTRY__(uhci, get_status)                    = &get_status; \
    __ENTRY__(uhci, remove_queue)                  = &remove_queue; \
    __ENTRY__(uhci, wait_poll)                     = &wait_poll; \
    __ENTRY__(uhci, traverse_skeleton)             = &traverse_skeleton; \
    __ENTRY__(uhci, create_USB_Packet)             = &create_USB_Packet; \
    __ENTRY__(uhci, dump_all)                      = &dump_all; \
    __ENTRY__(uhci, dump_skeleton)                 = &dump_skeleton; \
    __ENTRY__(uhci, inspect_TD)                    = &inspect_TD; \
    __ENTRY__(uhci, inspect_QH)                    = &inspect_QH; \
    __ENTRY__(uhci, inspect_transfer)              = &inspect_transfer; \
    __ENTRY__(uhci, print_USB_Transaction)         = &print_USB_Transaction; \
    __ENTRY__(uhci, print_USB_Transfer)            = &print_USB_Transfer; \
    __ENTRY__(uhci, control_transfer)              = &control_transfer; \
    __ENTRY__(uhci, interrupt_transfer)            = &interrupt_transfer; \
    __ENTRY__(uhci, bulk_transfer)                 = &bulk_transfer; \
    __ENTRY__(uhci, is_valid_priority)             = &is_valid_priority; \
    __ENTRY__(uhci, init_maps)                     = &init_maps; \
    __ENTRY__(uhci, create_dev)                    = &create_dev; \
    __ENTRY__(uhci, remove_transfer_entry)         = &remove_transfer_entry; \
    __ENTRY__(uhci, get_free_qh)                   = &get_free_qh; \
    __ENTRY__(uhci, get_free_td)                   = &get_free_td; \
    __ENTRY__(uhci, free_qh)                       = &free_qh; \
    __ENTRY__(uhci, free_td)                       = &free_td; \
    __ENTRY__(uhci, remove_td_linkage)             = &remove_td_linkage; \
    __ENTRY__(uhci, destroy_transfer)              = &destroy_transfer; \
    __ENTRY__(uhci, controller_configuration)      = &controller_configuration; \
    __ENTRY__(uhci, controller_port_configuration) = &controller_port_configuration; \
    __ENTRY__(uhci, is_valid_interval)             = &is_valid_interval; \
    __ENTRY__(uhci, controller_initializer)        = &controller_initializer; \
    __ENTRY__(uhci, controller_reset)              = &controller_reset; \
    __ENTRY__(uhci, build_data_stage_only)         = &build_data_stage_only; \
    __ENTRY__(uhci, build_setup_stage)             = &build_setup_stage; \
    __ENTRY__(uhci, build_data_stage)              = &build_data_stage; \
    __ENTRY__(uhci, build_data_transaction)        = &build_data_transaction; \
    __ENTRY__(uhci, build_status_stage)            = &build_status_stage; \
    __ENTRY__(uhci, successful_transmission_routine) = &successful_transmission_routine; \
    __ENTRY__(uhci, failed_transmission_routine)   = &failed_transmission_routine; \
    __ENTRY__(uhci, transmission_clearing_routine) = &transmission_clearing_routine; \
    __ENTRY__(uhci, __assign_fba)                  = &__assign_fba; \
    __ENTRY__(uhci, __init_buff_mem)               = &__init_buff_mem; \
    __ENTRY__(uhci, __uhci_build)                  = &__uhci_build; \
    __ENTRY__(uhci, __save_address)                = &__save_address; \
    __ENTRY__(uhci, __frame_entry)                 = &__frame_entry; \
    __ENTRY__(uhci, __build_qh)                    = &__build_qh; \
    __ENTRY__(uhci, __add_to_frame)                = &__add_to_frame; \
    __ENTRY__(uhci, __qh_set_parent)               = &__qh_set_parent; \
    __ENTRY__(uhci, __search_for_qh)               = &__search_for_qh; \
    __ENTRY__(uhci, __qh_inc_device_count)         = &__qh_inc_device_count; \
    __ENTRY__(uhci, __qh_dec_device_count)         = &__qh_dec_device_count; \
    __ENTRY__(uhci, __follow_schedule)             = &__follow_schedule; \
    __ENTRY__(uhci, __add_to_skeleton)             = &__add_to_skeleton; \
    __ENTRY__(uhci, __adjust_last_in_schedule)     = &__adjust_last_in_schedule; \
    __ENTRY__(uhci, __adjust_last_in_sub_schedule) = &__adjust_last_in_sub_schedule; \
    __ENTRY__(uhci, __retrieve_mqh)                = &__retrieve_mqh; \
    __ENTRY__(uhci, __adjust_remove)               = &__adjust_remove; \
    __ENTRY__(uhci, __packet_type)                 = &__packet_type; \
    __ENTRY__(uhci, __packet_type_control)         = &__packet_type_control; \
    __ENTRY__(uhci, __set_transaction_type)        = &__set_transaction_type; \
    __ENTRY__(uhci, __is_last_packet)              = &__is_last_packet; \
    __ENTRY__(uhci, __build_token)                 = &__build_token; \
    __ENTRY__(uhci, __save_map_properties)         = &__save_map_properties; \
    __ENTRY__(uhci, __save_map_properties_control) = &__save_map_properties_control; \
    __ENTRY__(uhci, __set_qhep)                    = &__set_qhep; \
    __ENTRY__(uhci, __set_flags)                   = &__set_flags; \
    __ENTRY__(uhci, __initial_state_routine)       = &__initial_state_routine; \
    __ENTRY__(uhci, __initial_state_routine_control) = &__initial_state_routine_control; \
    __ENTRY__(uhci, __default_qh)                  = &__default_qh; \
    __ENTRY__(uhci, __default_td)                  = &__default_td; \
    __ENTRY__(uhci, __build_td)                    = &__build_td; \
    __ENTRY__(uhci, __td_failed)                    = &__td_failed; \
    \
    __CALL_SUPER__(uhci->super, new_usb_controller, m, \
                    pci, UHCI_name)
    
#define __MQH_DEFAULT_FLAGS__(qh_type) \
    PRIORITY_QH_8 | QH_FLAG_END | QH_FLAG_IS_MQH | qh_type

#define __MQH_DEFAULT_QHEP__ \
    QH_TERMINATE | TD_SELECT

#define __BULK_DEFAULT__(uhci, qh) \
    __build_qh(uhci, qh, __MQH_DEFAULT_FLAGS__(QH_FLAG_TYPE_BULK), \
        QH_TERMINATE | QH_SELECT, __MQH_DEFAULT_QHEP__, 0)

#define __CTL_DEFAULT__(uhci, qh, bulk_phy) \
    __build_qh(uhci, qh, __MQH_DEFAULT_FLAGS__(QH_FLAG_TYPE_CONTROL), \
        (bulk_phy) | QH_SELECT, __MQH_DEFAULT_QHEP__, 0)

#define __INT_DEFAULT__(uhci, qh) \
    __build_qh(uhci, qh, __MQH_DEFAULT_FLAGS__(QH_FLAG_TYPE_INTERRUPT), \
        qh->pyhsicalQHLP, __MQH_DEFAULT_QHEP__, qh->parent)

#define __QH_BUILD_FLAGS__(qh, x) \
    __build_qh(qh, x, 0, 0, 0)
#define __QH_BUILD_QHLP__(qh, x) \
    __build_qh(qh, 0, x, 0, 0)
#define __QH_BUILD_QHEP__(qh, x) \
    __build_qh(qh, 0, 0, x, 0)
#define __QH_BUILD_PARENT__(qh, x) \
    __build_qh(qh, 0, 0, 0, x)

#define __GET_FREE_STRUCTURE_COND__(buffer, bit_map, type, uhci) \
    if(!bit_map[i]) { \
      __ARR_ENTRY__(bit_map, i, 1); \
      __USB_RELEASE__(__UHC_CAST__(uhci)); \
      return (type*)(buffer + (i * sizeof(type))); \
    }

#define __FREE_STRUCTURE_COND__(buffer, bit_map, type, name, uhci) \
    if((buffer + i) == (uint8_t*)name) { \
       __ARR_ENTRY__(bit_map, i / sizeof(type), 0); \
       __USB_RELEASE__(__UHC_CAST__(uhci)); \
       return; \
    }

#define __BODY_STRUCTURE_C__(cond) \
    __USB_LOCK__(__UHC_CAST__(uhci)); \
    cond \
    __USB_RELEASE__(__UHC_CAST__(uhci)); \

#define __GET_FREE_STRUCTURE__(type, size, buffer, bit_map, uhci) \
    __FOR_RANGE__(i, int, 0, (size / sizeof(type))) { \
      __BODY_STRUCTURE_C__(__GET_FREE_STRUCTURE_COND__(buffer, bit_map, \
        type, uhci)) \
    } \
    return (void*)0;

#define __FREE_STRUCTURE__(type, size, buffer, bit_map, name, uhci) \
    __FOR_RANGE_INC__(i, int, 0, size, sizeof(type)) { \
      __BODY_STRUCTURE_C__(__FREE_STRUCTURE_COND__(buffer, bit_map, \
        type, name, uhci)) \
    }

#define __TRANSFER_INITIALIZER__(controller, support_function, interface, \
  priority, callback) \
  int16_t shifted_prio; \
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super); \
  UsbDev *dev = \
    (UsbDev *)__STRUCT_CALL__(controller->interface_dev_map, get_c, interface); \
  __IF_SINGLE_RET__(__IS_NULL__(dev), callback(0, E_INTERFACE_NOT_SUPPORTED, data)) \
  __IF_SINGLE_RET__(!__STRUCT_CALL__(dev, support_function, interface), \
    callback(dev, E_NOT_SUPPORTED_TRANSFER_TYPE, data)) \
  __IF_SINGLE_RET__(__NEG_CHECK__((shifted_prio = \
    __STRUCT_CALL__(uhci, is_valid_priority, dev, priority, callback))), \
    callback(dev, E_PRIORITY_NOT_SUPPORTED, data))

#define __UHCI_CONTAINER__(controller, name) \
    _UHCI* name = (_UHCI*)container_of(controller, _UHCI, super)

#define __QHEP_ADDR__(qh) \
    qh->pyhsicalQHEP & QH_ADDRESS_MASK

#define __QHLP_ADDR__(qh) \
    qh->pyhsicalQHLP & QH_ADDRESS_MASK

#define __LP_ADDR__(td) \
    td->pyhsicalLinkPointer & QH_ADDRESS_MASK

struct _UHCI {
  struct UsbController super;
  void (*new_UHCI)(struct _UHCI *uhci, PciDevice_Struct* pci_device, 
    SystemService_C* mem_service);
  QH* (*request_frames)(struct _UHCI* uhci);
  void (*insert_queue)(struct _UHCI *uhci, struct QH *new_qh,
                       uint16_t priority, enum QH_HEADS v);
  unsigned int (*retransmission)(struct _UHCI *uhci, struct QH *process_qh);
  uint32_t (*get_status)(struct _UHCI *uhci, struct TD *td);
  void (*remove_queue)(struct _UHCI *uhci, struct QH *qh);
  uint32_t (*wait_poll)(struct _UHCI *uhci, struct QH *process_qh, uint32_t timeout, 
                        uint8_t flags);
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
  void (*init_maps)(struct _UHCI *uhci, MemoryService_C *m);
  void (*create_dev)(struct _UHCI *uhci, int16_t status, int pn,
                     MemoryService_C *m);
  void (*remove_transfer_entry)(struct _UHCI *uhci, QH *entry);
  void (*remove_td_linkage)(struct _UHCI *uhci, TD *start);
  int (*supported_event_listener_type)(struct _UHCI *uhci,
                                       uint16_t event_listener_type);
  void (*dump_uhci_entry)(struct _UHCI* uhci);                                       
  void (*destroy_transfer)(struct _UHCI* uhci, UsbTransfer* transfer);
  void (*controller_port_configuration)(struct _UHCI* uhci);
  int (*controller_configuration)(struct _UHCI* uhci);
  int16_t (*is_valid_interval)(struct _UHCI* uhci, UsbDev* dev, uint16_t interval, void* data);
  int (*controller_initializer)(struct _UHCI* uhci);
  int (*controller_reset)(struct _UHCI* uhci);
  UsbTransaction* (*build_setup_stage)(struct _UHCI* uhci, UsbDeviceRequest* device_request, 
    UsbDev* dev, uint8_t endpoint, uint8_t flags);
  UsbTransaction* (*build_data_stage)(struct _UHCI* uhci, UsbDev* dev, uint16_t total_bytes_to_transfer,
    uint8_t packet_type, void* data, uint8_t endpoint, UsbTransaction* prev_trans,
    unsigned int *count, uint8_t flags);
  UsbTransaction* (*build_data_stage_only)(struct _UHCI* uhci, void* data, unsigned int len,
    Endpoint* e, UsbDev* dev, unsigned int* count, uint8_t flags);
  UsbTransaction* (*build_data_transaction)(struct _UHCI* uhci, unsigned int len,
    uint8_t packet_type, uint8_t* start, uint8_t* end, uint16_t max_len, UsbDev* dev,
    uint8_t endpoint, UsbPacket* prev, uint8_t flags, uint8_t toggle);
  void (*build_status_stage)(struct _UHCI* uhci, uint8_t packet_type, uint8_t endpoint,
    UsbDev* dev, UsbTransaction* prev_trans);
  void (*successful_transmission_routine)(struct _UHCI* uhci, callback_function callback, 
    UsbDev* dev, void* data, QH* entry, MemoryService_C* mem_service);
  void (*failed_transmission_routine)(struct _UHCI* uhci, UsbDev* dev, void* data, 
    TD* td, QH* entry, MemoryService_C* mem_service, callback_function callback);
  void (*transmission_clearing_routine)(struct _UHCI* uhci, QH* entry, 
    MemoryService_C* mem_service);
  void (*__assign_fba)(struct _UHCI* uhci, MemoryService_C* m, uint32_t* frame_list);
  void (*__init_buff_mem)(struct _UHCI* uhci, uint32_t qh_size, uint32_t td_size);
  void (*__uhci_build)(struct _UHCI* uhci, MemoryService_C* m, PciDevice_Struct* pci_device,
    uint32_t qh_size, uint32_t td_size);
  void (*__save_address)(struct _UHCI* uhci, MemoryService_C* m, QH** physical_addr, QH* current, 
                     int pos);
  void (*__frame_entry)(struct _UHCI* uhci, MemoryService_C* m, QH* next, QH* current, 
                                 uint32_t next_physical_addr, 
                                 uint32_t* frame_list, int fn);
  void (*__build_qh)(struct _UHCI* uhci, QH* qh, uint32_t flags, uint32_t qhlp,
                              uint32_t qhep, uint32_t parent);
  void (*__add_to_frame)(struct _UHCI* uhci, uint32_t* frame_list, int fn,
                                  QH* physical);
  void (*__qh_set_parent)(struct _UHCI* uhci, MemoryService_C* m, QH* qh, QH* parent);
  QH* (*__search_for_qh)(struct _UHCI* uhci, MemoryService_C* m, QH* current, enum QH_HEADS v);
  void (*__qh_inc_device_count)(struct _UHCI* uhci, QH* current);
  void (*__qh_dec_device_count)(struct _UHCI* uhci, QH* current);
  QH* (*__follow_schedule)(struct _UHCI* uhci, MemoryService_C* m, QH* current, uint16_t priority);
  void (*__add_to_skeleton)(struct _UHCI* uhci, MemoryService_C* m, QH* current, QH* new_qh);
  void (*__adjust_last_in_schedule)(struct _UHCI* uhci, QH* current, QH* new_qh);
  void (*__adjust_last_in_sub_schedule)(struct _UHCI* uhci, MemoryService_C* m, QH* current, QH* new_qh);
  QH* (*__retrieve_mqh)(struct _UHCI* uhci, MemoryService_C* m, QH* start_point);
  void (*__adjust_remove)(struct _UHCI* uhci, QH* qh, QH* parent, QH* child);
  uint8_t (*__packet_type)(struct _UHCI* uhci, UsbDev* dev, Endpoint* e);
  uint8_t (*__packet_type_control)(struct _UHCI* uhci, UsbDev* dev, UsbDeviceRequest* request);
  void (*__set_transaction_type)(struct _UHCI* uhci, UsbTransaction* transaction,
    uint8_t packet_type);
  int (*__is_last_packet)(struct _UHCI* uhci, uint8_t* current_data, uint8_t* end, 
    uint16_t max_len);
  TokenValues (*__build_token)(struct _UHCI* uhci, uint16_t max_len, uint8_t toggle,
    uint8_t endpoint_number, uint8_t address_number, uint16_t packet_type);
  void (*__save_map_properties)(struct _UHCI* uhci, QH* qh, 
    TD* td, void* data, UsbDev* dev, callback_function callback);
  void (*__save_map_properties_control)(struct _UHCI* uhci, QH* qh, 
    TD* td, void* data, UsbDev* dev, callback_function callback, 
    UsbDeviceRequest* request);
  void (*__set_qhep)(struct _UHCI* uhci, QH* qh, TD* td, MemoryService_C* m);
  void (*__set_flags)(struct _UHCI* uhci, QH* qh, uint32_t type, 
    uint32_t transaction_count);
  void (*__initial_state_routine)(struct _UHCI* uhci, uint32_t timeout,
    QH* qh, uint8_t flags, UsbDev* dev, void* data, uint32_t qh_physical, TD* td,
    MemoryService_C* m, callback_function callback);
  void (*__initial_state_routine_control)(struct _UHCI* uhci, uint32_t timeout,
    QH* qh, uint8_t flags, UsbDev* dev, void* data, uint32_t qh_physical, TD* td,
    MemoryService_C* m, UsbDeviceRequest* rq, callback_function callback);
  void (*__default_qh)(struct _UHCI* uhci, QH* qh);
  void (*__default_td)(struct _UHCI* uhci, TD* td);
  TD* (*__build_td)(struct _UHCI* uhci, UsbDev* dev, 
    UsbPacket* prev, TokenValues* token,
    int8_t speed, void* data, int last_packet, uint8_t flags);
  uint8_t (*__td_failed)(struct _UHCI* uhci, TD* td);

  QH *qh_entry;
  uint32_t fba;
  SuperMap *qh_to_td_map;
  SuperMap *callback_map;
  SuperMap *qh_data_map;
  SuperMap *qh_dev_map;
  SuperMap *qh_device_request_map;
  uint8_t signal;
  uint8_t signal_not_override;
  uint8_t *map_io_buffer_qh;
  uint8_t *map_io_buffer_td; 
  uint8_t qh_len; // default 1 page
  uint8_t td_len; // default 2 pages
  uint8_t* map_io_buffer_bit_map_qh;
  uint8_t* map_io_buffer_bit_map_td;

  #if defined(TRANSFER_MEASURE_ON)
  SuperMap * qh_measurement;
  #endif
};

typedef struct _UHCI _UHCI;

void new_UHCI(struct _UHCI *uhci, PciDevice_Struct* pci_device, SystemService_C *mem_service);

typedef UsbTransfer *(*build_control_transfer)(
    _UHCI *uhci, UsbDev *dev, struct UsbDeviceRequest *device_request,
    void *data, Endpoint *endpoint, uint8_t flags);
typedef UsbTransfer *(*build_bulk_or_interrupt_transfer)(
    _UHCI *uhci, UsbDev *dev, void *data, Endpoint *e, unsigned int len,
    const char *type, uint8_t flags);

extern const uint8_t CLASS_ID;
extern const uint8_t SUBCLASS_ID;
extern const uint8_t INTERFACE_ID;


#define __DECLARE_UHCI_DEFAULT__(uhci, mem_service, pci) \
    __uhci_build(uhci, mem_service, pci, PAGE_SIZE, 2*PAGE_SIZE)

#define __DECLARE_UHCI_LOW__(uhci, mem_service, pci) \
    __uhci_build(uhci, mem_service, pci, PAGE_SIZE, PAGE_SIZE)

#define __DECLARE_UHCI_HIGH__(uhci, mem_service, pci) \
    __uhci_build(uhci, mem_service, pci, 2*PAGE_SIZE, 4*PAGE_SIZE)

#define __DECLARE_UHCI_CUSTOM__(uhci, mem_service, pci, qh_size, td_size) \
    __uhci_build(uhci, mem_service, pci, qh_size, td_size)

#endif