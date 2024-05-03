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
    \
    __CALL_SUPER__(uhci->super, new_usb_controller, m, \
                    pci, UHCI_name)

#define __ADD_REGISTER_ENTRIES__(array) \
    __ARR_ENTRY__(array, 0, (Register *)c_reg); \
    __ARR_ENTRY__(array, 1, (Register *)s_reg); \
    __ARR_ENTRY__(array, 2, (Register *)i_reg); \
    __ARR_ENTRY__(array, 3, (Register *)f_n_reg); \
    __ARR_ENTRY__(array, 4, (Register *)f_b_reg); \
    __ARR_ENTRY__(array, 5, (Register *)sof_reg); \
    __ARR_ENTRY__(array, 6, (Register *)p_reg_1); \
    __ARR_ENTRY__(array, 7, (Register *)p_reg_2)

#define __MQH_DEFAULT_FLAGS__(qh_type) \
    PRIORITY_QH_8 | QH_FLAG_END | QH_FLAG_IS_MQH | qh_type

#define __MQH_DEFAULT_QHEP__ \
    QH_TERMINATE | TD_SELECT

#define __BULK_DEFAULT__(qh) \
    __build_qh(qh, __MQH_DEFAULT_FLAGS__(QH_FLAG_TYPE_BULK), \
        QH_TERMINATE | QH_SELECT, __MQH_DEFAULT_QHEP__, 0)

#define __CTL_DEFAULT__(qh, bulk_phy) \
    __build_qh(qh, __MQH_DEFAULT_FLAGS__(QH_FLAG_TYPE_CONTROL), \
        (bulk_phy) | QH_SELECT, __MQH_DEFAULT_QHEP__, 0)

#define __INT_DEFAULT__(qh) \
    __build_qh(qh, __MQH_DEFAULT_FLAGS__(QH_FLAG_TYPE_INTERRUPT), \
        qh->pyhsicalQHLP, __MQH_DEFAULT_QHEP__, qh->parent)

#define __QH_BUILD_FLAGS__(qh, x) \
    __build_qh(qh, x, 0, 0, 0)
#define __QH_BUILD_QHLP__(qh, x) \
    __build_qh(qh, 0, x, 0, 0)
#define __QH_BUILD_QHEP__(qh, x) \
    __build_qh(qh, 0, 0, x, 0)
#define __QH_BUILD_PARENT__(qh, x) \
    __build_qh(qh, 0, 0, 0, x)

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