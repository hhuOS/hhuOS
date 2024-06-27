#include "UHCI.h"
#include "../../dev/UsbDevice.h"
#include "../../dev/data/UsbDev_Data.h"
#include "../../driver/UsbDriver.h"
#include "../../events/EventDispatcher.h"
#include "../../include/UsbControllerInclude.h"
#include "../../include/UsbErrors.h"
#include "../../interfaces/IoPortInterface.h"
#include "../../interfaces/LoggerInterface.h"
#include "../../interfaces/MutexInterface.h"
#include "../../interfaces/PciDeviceInterface.h"
#include "../../interfaces/PciInterface.h"
#include "../../interfaces/SystemInterface.h"
#include "../../interfaces/ThreadInterface.h"
#include "../../interfaces/TimeInterface.h"
#include "../../utility/Utils.h"
#include "../UsbController.h"
#include "../UsbControllerFlags.h"
#include "../components/ControllerMemory.h"
#include "../components/ControllerMemoryTypes.h"
#include "../components/ControllerRegister.h"
#include "components/UHCIRegister.h"
#include "data/UHCI_Data.h"

static QH* request_frames(struct _UHCI* uhci);
static Addr_Region *i_o_space_layout_run(UsbController* controller, PciDevice_Struct* pci_device);
static Register** request_register(UsbController* controller, Addr_Region* addr_region);
static uint32_t bulk_entry_point_uhci(struct UsbDev *dev, Interface* interface, Endpoint *endpoint, void *data,
                      unsigned int len, uint8_t priority,
                      callback_function callback, uint8_t flags);
static uint32_t control_entry_point_uhci(struct UsbDev *dev,
                         struct UsbDeviceRequest *device_request, void *data,
                         uint8_t priority, Interface* interface, Endpoint *endpoint,
                         callback_function callback, uint8_t flags);
static uint32_t interrupt_entry_point_uhci(struct UsbDev *dev, Interface* interface, Endpoint *endpoint, void *data,
                           unsigned int len, uint8_t priority,
                           uint16_t interval, callback_function callback);
static uint32_t iso_entry_point_uhci(UsbDev* dev, Interface* interface, Endpoint* endpoint, void* data, 
  unsigned int len, uint8_t priority, uint16_t interval, callback_function callback);
static void insert_queue(struct _UHCI *uhci, struct QH *new_qh,
                  uint16_t priority, enum QH_HEADS v);
static int remove_transfer_uhci(UsbController* controller, uint32_t transfer_id);
static void remove_queue(struct _UHCI *uhci, struct QH *qh);
static uint32_t control_transfer(_UHCI *uhci, UsbDev *dev, struct UsbDeviceRequest *rq,
                      void *data, uint8_t priority, Interface* interface, Endpoint *endpoint,
                      callback_function callback, uint8_t flags);
static uint32_t interrupt_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                        uint16_t interval, uint8_t priority, Interface* interface, Endpoint *e, callback_function callback);
static uint32_t bulk_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                   uint8_t, Interface* interface, Endpoint *e, callback_function callback, uint8_t flags);
static uint32_t iso_transfer(_UHCI* uhci, UsbDev* dev, void* data, unsigned int len,
  uint16_t interval, uint8_t priority, Interface* interface, Endpoint *e, callback_function callback, uint16_t flags);
static uint32_t wait_poll(_UHCI *uhci, QH *process_qh, uint32_t timeout, uint8_t flags);
static void _poll_uhci_(UsbController *controller);
static QH *get_free_qh(_UHCI *uhci, uint32_t* qh_id);
static TD *get_free_td(_UHCI *uhci, uint32_t* td_id);
static void free_qh(_UHCI *uhci, QH *qh);
static void free_td(_UHCI *uhci, TD *td);
static uint32_t init_control_transfer(UsbController *controller, Interface *interface,
                           unsigned int pipe, uint8_t priority, void *data,
                           uint8_t *setup, callback_function callback);
static uint32_t init_interrupt_transfer(UsbController *controller, Interface *interface,
                             unsigned int pipe, uint8_t priority, void *data,
                             unsigned int len, uint16_t interval,
                             callback_function callback);
static uint32_t init_bulk_transfer(UsbController *controller, Interface *interface,
                        unsigned int pipe, uint8_t priority, void *data,
                        unsigned int len, callback_function callback);
static uint32_t init_iso_transfer(UsbController* controller, Interface* interface,
  unsigned int pipe, uint8_t priority, void* data, unsigned int len, 
  uint16_t interval, callback_function callback);
static unsigned int retransmission(_UHCI *uhci, struct QH *process_qh);
static void traverse_skeleton(_UHCI *uhci, struct QH *entry);
static uint32_t get_status(_UHCI *uhci, TD *td);
static UsbPacket *create_USB_Packet(_UHCI *uhci, UsbDev *dev, UsbPacket *prev,
                             struct TokenValues token, int8_t speed, void *data,
                             int last_packet, uint8_t flags);
static UsbTransfer *build_control(_UHCI *uhci, UsbDev *dev,
                           UsbDeviceRequest *device_request, void *data,
                           Endpoint *e, uint8_t flags);
static UsbTransfer *build_other_type_transfer(_UHCI *uhci, UsbDev *dev, void *data,
                                     Endpoint *e, unsigned int len,
                                     const char *type, uint8_t flags);
static int uhci_contain_interface(UsbController *controller, Interface *interface);
static int16_t is_valid_priority(_UHCI *uhci, UsbDev *dev, uint8_t priority);
static UsbControllerType is_of_type_uhci(UsbController *controller);
static uint16_t uhci_reset_port(UsbController *controller, uint8_t port);
static void init_maps(_UHCI *uhci, MemoryService_C *m);
static void create_dev(_UHCI *uhci, int16_t status, int pn, MemoryService_C *m);
static void remove_transfer_entry(_UHCI *uhci, QH *entry);
static void dump_all(_UHCI *uhci);
static void dump_skeleton(_UHCI *uhci);
static void inspect_TD(_UHCI *uhci, struct TD *td);
static void inspect_QH(_UHCI *uhci, struct QH *qh);
static void inspect_transfer(_UHCI *uhci, struct QH *qh, struct TD *td);
static void print_USB_Transaction(_UHCI *uhci, UsbTransaction *transaction, int order);
static void print_USB_Transfer(_UHCI *uhci, UsbTransfer *transfer);
static void remove_td_linkage(_UHCI *uhci, TD *start);
static void dump_uhci_entry(_UHCI* uhci);
static void handler_function_uhci(UsbController* controller);
static void runnable_function_uhci(UsbController* controller);
static void destroy_transfer(_UHCI* uhci, UsbTransfer* transfer);
static void controller_port_configuration(_UHCI* uhci);
static int controller_configuration(_UHCI* uhci);
static int16_t is_valid_interval(_UHCI* uhci, UsbDev* dev, uint16_t interval);
static int controller_initializer(_UHCI* uhci);
static int controller_reset(_UHCI* uhci);
static UsbTransaction* build_setup_stage(_UHCI* uhci, UsbDeviceRequest* device_request, 
  UsbDev* dev, uint8_t endpoint, uint8_t flags);
static UsbTransaction* build_data_stage(_UHCI* uhci, UsbDev* dev, uint16_t total_bytes_to_transfer,
  uint8_t packet_type, void* data, uint8_t endpoint, UsbTransaction* prev_trans,
  unsigned int *count, uint8_t flags);
static UsbTransaction* build_data_stage_only(_UHCI* uhci, void* data, unsigned int len,
  Endpoint* e, UsbDev* dev, unsigned int* count, uint8_t flags);
static UsbTransaction* build_data_transaction(_UHCI* uhci, unsigned int len,
  uint8_t packet_type, uint8_t* start, uint8_t* end, uint16_t max_len, UsbDev* dev,
  uint8_t endpoint, UsbPacket* prev, uint8_t flags, uint8_t toggle);
static void build_status_stage(_UHCI* uhci, uint8_t packet_type, uint8_t endpoint,
  UsbDev* dev, UsbTransaction* prev_trans);
static void successful_transmission_routine(_UHCI* uhci, callback_function callback, 
  UsbDev* dev, Interface* interface, void* data, QH* entry, MemoryService_C* mem_service);
static void failed_transmission_routine(_UHCI* uhci, UsbDev* dev, Interface* interface, 
  void* data, TD* td, QH* entry, MemoryService_C* mem_service, callback_function callback);
static void transmission_clearing_routine(_UHCI* uhci, QH* entry, 
  MemoryService_C* mem_service);
static int fast_buffer_change_uhci(struct UsbController* controller, UsbDev* dev, 
  Endpoint* endpoint, uint32_t qh_id, void* buffer);
static uint32_t iso_transfer_ext(UsbController* uhci, Interface* interface, 
  Endpoint* e, void* data, unsigned int len,
  uint16_t interval, uint8_t priority, callback_function callback);
static int reset_transfer_uhci(UsbController* controller, uint32_t transfer_id);

static inline void __assign_fba(_UHCI* uhci, MemoryService_C* m, 
                                uint32_t* frame_list) {
    uhci->fba = 
        __PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, frame_list));
}   

static inline void __init_buff_mem(_UHCI* uhci, uint32_t qh_size, uint32_t td_size) {
    __mem_set(uhci->map_io_buffer_qh, qh_size, 0); 
    __mem_set(uhci->map_io_buffer_td, td_size, 0); 
    __mem_set(uhci->map_io_buffer_bit_map_qh, qh_size / sizeof(QH), 0);
    __mem_set(uhci->map_io_buffer_bit_map_td, td_size / sizeof(TD), 0);
}

static inline void __save_address(_UHCI* uhci, MemoryService_C* m, QH** physical_addr, 
  QH* current, int pos) {
  uint32_t physical;
  QH* qh__ = (QH*)__GET_PHYSICAL__(m, current);
  __ARR_ENTRY__(physical_addr, pos, qh__);
  physical = __PTR_TYPE__(uint32_t, physical_addr[pos]);
  __STRUCT_CALL__(m, addVirtualAddress, physical, current);
}

static inline void __frame_entry(_UHCI* uhci, MemoryService_C* m, QH* next, QH* current, 
                                 uint32_t next_physical_addr, 
                                 uint32_t* frame_list, int fn) {
  current->pyhsicalQHLP = next_physical_addr | QH_SELECT;
  next->parent = __PTR_TYPE__(uint32_t, 
      __GET_PHYSICAL__(m, current));
  frame_list[fn] = __PTR_TYPE__(uint32_t, 
      __GET_PHYSICAL__(m, current)) | QH_SELECT;
}

static inline void __build_qh(_UHCI* uhci, QH* qh, uint32_t flags, uint32_t qhlp,
                              uint32_t qhep, uint32_t parent) {
  qh->flags = flags;
  qh->pyhsicalQHLP = qhlp;
  qh->pyhsicalQHEP = qhep;
  qh->parent = parent;
}

static inline void __add_to_frame(_UHCI* uhci, uint32_t* frame_list, int fn,
                                  QH* physical) {
  frame_list[fn] = 
      (__PTR_TYPE__(uint32_t, physical)) | QH_SELECT;
}

static inline void __qh_set_parent(_UHCI* uhci, MemoryService_C* m, QH* qh, QH* parent) {
  qh->parent = __PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, parent));
}

static inline QH* __search_for_qh(_UHCI* uhci, MemoryService_C* m, QH* current, enum QH_HEADS v){
  uint32_t physical_addr;
  int8_t offset = (int8_t)v;

  while (offset > 0) {
    if ((current->flags & QH_FLAG_IS_MQH)) {
      offset--;
    }
    physical_addr = (current->pyhsicalQHLP & QH_ADDRESS_MASK);
    current = __GET_VIRTUAL__(m, physical_addr, QH);
  }
  return current;
}

// @deprecated
/*static inline void __qh_inc_device_count(_UHCI* uhci, QH* current) {
  current->flags = ((((current->flags & QH_FLAG_DEVICE_COUNT_MASK) >>
                      QH_FLAG_DEVICE_COUNT_SHIFT) +
                     1)
                    << QH_FLAG_DEVICE_COUNT_SHIFT) |
                   (current->flags & 0xFF);
}*/

// @deprecated
/*static inline void __qh_dec_device_count(_UHCI* uhci, QH* current) {
  current->flags = ((((current->flags & QH_FLAG_DEVICE_COUNT_MASK) >>
                  QH_FLAG_DEVICE_COUNT_SHIFT) -
                 1)
                << QH_FLAG_DEVICE_COUNT_SHIFT) |
               (current->flags & 0xFF);
} */

static inline void __save_QH_ID(_UHCI* uhci, QH* qh, uint32_t qh_id){
  qh->flags |= (qh_id << QH_ID_SHIFT);
}

static inline QH* __follow_schedule(_UHCI* uhci, MemoryService_C* m, QH* current, 
  uint16_t priority, uint8_t type_flag) {
  while (((current->flags & QH_FLAG_END_MASK) == QH_FLAG_IN) &&
         ((current->flags & PRIORITY_QH_MASK) >= priority)) {
    if(((current->flags & QH_FLAG_IS_MASK) == QH_FLAG_IS_QH) && 
      ((current->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_INTERRUPT) &&
      type_flag == QH_FLAG_TYPE_ISO) return  (__GET_VIRTUAL__(m, current->parent, QH));
    current = __GET_VIRTUAL__(m, __QHLP_ADDR__(current), QH); 
  }
  if(((current->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_INTERRUPT)
    && (type_flag == QH_FLAG_TYPE_ISO)){
    current = (__GET_VIRTUAL__(m, current->parent, QH));
  }
  return current;
}

static inline void __add_to_skeleton(_UHCI* uhci, MemoryService_C* m, QH* current, QH* new_qh) {
  current->pyhsicalQHLP = 
    (__PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, new_qh))) | QH_SELECT;
}

static inline void __adjust_last_in_schedule(_UHCI* uhci, QH* current, QH* new_qh) {
  if ((current->pyhsicalQHLP & QH_TERMINATE) ==
      QH_TERMINATE) { // erase bit which got set prior
    current->pyhsicalQHLP &= 0xFFFFFFFE;
    new_qh->pyhsicalQHLP = QH_TERMINATE | QH_SELECT;
  } else {
    new_qh->pyhsicalQHLP = current->pyhsicalQHLP;
  }
}

static inline void __adjust_last_in_sub_schedule(_UHCI* uhci, MemoryService_C* m, QH* current, QH* new_qh) {
  if ((current->flags & QH_FLAG_END_MASK) == QH_FLAG_END) {
    new_qh->flags |= QH_FLAG_END;
    current->flags &= 0xFFFFFFFE;
  } else { // if not last element in chain update parent pointer
    QH *c = __GET_VIRTUAL__(m, __QHLP_ADDR__(current), QH);
    c->parent = __PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, new_qh));
  }
}

static inline QH* __retrieve_mqh(_UHCI* uhci, MemoryService_C* m, QH* start_point) {
  QH *current = start_point;
  while ((current->flags & QH_FLAG_IS_MASK) == QH_FLAG_IS_QH) {
    current = __GET_VIRTUAL__(m, current->parent, QH);
  }
  return current;
}

static inline void __adjust_remove(_UHCI* uhci, QH* qh, QH* parent, QH* child) {
  __IF_CUSTOM__((qh->flags & QH_FLAG_END_MASK) == QH_FLAG_END, 
    parent->flags |= QH_FLAG_END);
  __IF_CUSTOM__((qh->flags & QH_FLAG_END_MASK) == QH_FLAG_IN,
    child->parent = qh->parent); // if qh is not last in chain update child pointer to
}

static inline void __set_transaction_type(_UHCI* uhci, UsbTransaction* transaction,
  uint8_t packet_type) {
  transaction->transaction_type = 
    __IF_EXT__((packet_type == OUT), DATA_OUT_TRANSACTION, DATA_IN_TRANSACTION);
}

static inline int __is_last_packet(_UHCI* uhci, uint8_t* current_data, uint8_t* end, 
  uint16_t max_len) {
  return __IF_EXT__((current_data + max_len) >= end, 1, 0);
}

static inline TokenValues __build_token(_UHCI* uhci, uint16_t max_len, uint8_t toggle,
  uint8_t endpoint_number, uint8_t address_number, uint16_t packet_type) {
  return (TokenValues){.max_len = max_len,
                       .toggle = toggle,
                       .endpoint = endpoint_number,
                       .address = address_number,
                       .packet_type = packet_type};
}

static inline void __save_map_properties(_UHCI* uhci, QH* qh, 
  TD* td, void* data, UsbDev* dev, Interface* interface, 
  callback_function callback) {
  __STRUCT_CALL__(uhci->qh_to_td_map, put_c, qh, td);
  __STRUCT_CALL__(uhci->qh_data_map, put_c, qh, data);
  __STRUCT_CALL__(uhci->qh_dev_map, put_c, qh, dev);
  __STRUCT_CALL__(uhci->qh_interface_map, put_c, qh, interface);
  __STRUCT_CALL__(uhci->callback_map, put_c, qh, callback);
}

static inline void __save_map_properties_control(_UHCI* uhci, QH* qh, 
  TD* td, void* data, UsbDev* dev, Interface* interface, callback_function callback, 
  UsbDeviceRequest* request) {
  __STRUCT_CALL__(uhci, __save_map_properties, qh, td, data, dev, interface, callback);
  __STRUCT_CALL__(uhci->qh_device_request_map, put_c, qh, request);
}

static inline void __set_qhep(_UHCI* uhci, QH* qh, TD* td, MemoryService_C* m){
  qh->pyhsicalQHEP = __PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, td));
}

static inline void __set_flags(_UHCI* uhci, QH* qh, uint32_t type, 
  uint32_t qh_id, uint32_t iso_ext) {
  qh->flags = (type | (qh_id << QH_ID_SHIFT) | iso_ext);
}

static inline void __initial_state_routine(_UHCI* uhci, uint32_t timeout,
  QH* qh, uint8_t flags, UsbDev* dev, Interface* interface, 
  void* data, uint32_t qh_physical, TD* td,
  MemoryService_C* m, callback_function callback){
  uint32_t status = __STRUCT_CALL__(uhci, wait_poll, qh, timeout, 
    flags & SUPRESS_DEVICE_ERRORS);

  callback(dev, interface, status, data);

  __STRUCT_CALL__(uhci, remove_queue, qh);
  __STRUCT_CALL__(m, remove_virtualAddress, qh_physical);
  __STRUCT_CALL__(uhci, free_qh, qh);
  __STRUCT_CALL__(uhci, remove_td_linkage, td);
}

static inline void __initial_state_routine_control(_UHCI* uhci, uint32_t timeout,
  QH* qh, uint8_t flags, UsbDev* dev, Interface* interface, void* data, uint32_t qh_physical, TD* td,
  MemoryService_C* m, UsbDeviceRequest* rq, callback_function callback) {
  __STRUCT_CALL__(uhci, __initial_state_routine, timeout, qh, flags, dev, interface, data,
    qh_physical, td, m, callback);
  __STRUCT_CALL__(dev, free_device_request, rq);
}

static inline void __default_qh(_UHCI* uhci, QH* qh) {
  qh->pyhsicalQHLP = 0;
  qh->pyhsicalQHEP = 0;
  qh->parent       = 0;
  qh->flags        = 0;
}

static inline void __default_td(_UHCI* uhci, TD* td) {
  td->pyhsicalLinkPointer = 0;
  td->control_x_status    = 0;
  td->token               = 0;
  td->bufferPointer       = 0;
}

static inline uint8_t __packet_type_control(_UHCI* uhci, UsbDev* dev, 
  UsbDeviceRequest* request) {
  return __IF_EXT__(__STRUCT_CALL__(dev, __is_device_to_host, request), IN, OUT);
}

static inline uint8_t __packet_type(_UHCI* uhci, UsbDev* dev, Endpoint* e) {
  return __IF_EXT__(__STRUCT_CALL__(dev,__is_direction_in, e), IN, OUT);
}

static inline TD* __build_td(_UHCI* uhci, UsbDev* dev, 
  UsbPacket* prev, TokenValues* token,
  int8_t speed, void* data, int last_packet, uint8_t flags) {
  __UHC_MEMORY__(uhci, m);
  uint32_t td_id;
  TD *td = __STRUCT_CALL__(uhci, get_free_td, &td_id);
  __STRUCT_CALL__(uhci, __default_td, td);
  __IF_COND__(__NOT_NULL__(prev)) {
    prev->internalTD->pyhsicalLinkPointer |= 
      (__PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, td)));
  }

  td->pyhsicalLinkPointer = DEPTH_BREADTH_SELECT | TD_SELECT;
  td->control_x_status = __IF_EXT__((speed == LOW_SPEED), 
    (0x1 << LS), (0x0 << LS));
  td->control_x_status |= (0x1 << ACTIVE);
  td->control_x_status |= (0x3 << C_ERR); // 3 Fehlversuche max

  if((flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_ISO){
    td->control_x_status |= (0x1 << IOS);
  }

  __IF_COND__(last_packet){
    td->pyhsicalLinkPointer |= QH_TERMINATE;
    __IF_COND__(__CONFIG_STATE__(dev) && __BULK_NOT_INITIAL__(flags) && 
      __CTL_NOT_INITIAL__(flags)){
      td->control_x_status |= (0x1 << IOC); 
    }
  }

  td->token = ((token->max_len - 1) & 0x7FF) << TD_MAX_LENGTH;
  td->token |= (token->toggle << TD_DATA_TOGGLE);
  td->token |= (token->endpoint << TD_ENDPOINT);
  td->token |= (token->address << TD_DEVICE_ADDRESS);
  td->token |= (token->packet_type);

  td->bufferPointer = __PTR_TYPE__(uint32_t,__GET_PHYSICAL__(m, data));

  return td;
}

static inline uint8_t __td_failed(_UHCI* uhci, TD* td) {
  return ((td->control_x_status >> ACTIVE) & 0x01);
}

static inline int8_t __match_qh_by_id(_UHCI* uhci, uint32_t transfer_id, QH* entry){
  uint32_t qh_id = (entry->flags & QH_ID_MASK) >> QH_ID_SHIFT;
  return __IF_EXT__((qh_id == transfer_id), 1, -1);
}

static inline QH* __find_qh_in_skeleton_by_id(_UHCI* uhci, uint32_t transfer_id){
  QH* current = uhci->qh_entry;
  __MEM_SERVICE__(((UsbController*)uhci)->mem_service, m);
  while(__NOT_NULL__(current)){
    if(__STRUCT_CALL__(uhci, __match_qh_by_id, transfer_id, current) == 1){
      return current;
    }
    current = __GET_VIRTUAL__(m, __QHLP_ADDR__(current), QH);
  }
  return (void*)0;
}

static inline void __switch_buffer(struct _UHCI* uhci, void* buffer, TD* td){
  __MEM_SERVICE__(((UsbController*)uhci)->mem_service, m);
  td->bufferPointer = __PTR_TYPE__(uint32_t,__GET_PHYSICAL__(m, buffer));
}

static inline void __uhci_build(_UHCI* uhci, MemoryService_C* m, PciDevice_Struct* pci_device,
                                uint32_t qh_size, uint32_t td_size) {
    __INIT_UHCI__(uhci, (SystemService_C*)m, pci_device);
    uhci->qh_len = qh_size;
    uhci->td_len = td_size;
    uhci->map_io_buffer_qh = __MAP_IO_KERNEL__(m, uint8_t, qh_size / PAGE_SIZE);
    uhci->map_io_buffer_bit_map_qh = __ALLOC_KERNEL_MEM__(m, uint8_t, qh_size / sizeof(QH));
    
    uhci->map_io_buffer_td = __MAP_IO_KERNEL__(m, uint8_t, td_size / PAGE_SIZE);
    uhci->map_io_buffer_bit_map_td = __ALLOC_KERNEL_MEM__(m, uint8_t, td_size / sizeof(TD));
    
    __STRUCT_CALL__(uhci, __init_buff_mem, qh_size, td_size);

    uhci->signal = 0;
    uhci->signal_not_override = 0;
}

const uint8_t CLASS_ID = 0x0C;
const uint8_t SUBCLASS_ID = 0x03;
const uint8_t INTERFACE_ID = 0x00;

// first two root ports are just hid ; later we can replace it with hubs and
// place hid to the hubs
void new_UHCI(_UHCI *uhci, PciDevice_Struct *pci_device,
              SystemService_C *mem_service) {
  __MEM_SERVICE__(mem_service, m);

  __DECLARE_UHCI_DEFAULT__(uhci, m, pci_device);

  __STRUCT_CALL__(uhci, dump_uhci_entry);
  uhci->qh_entry = __STRUCT_CALL__(uhci, request_frames);
  __STRUCT_CALL__(uhci, init_maps, m);

  __add_look_up_registers(__UHC_CAST__(uhci), 8);

  __IF_CUSTOM__(__STRUCT_CALL__(uhci, controller_configuration), 
    create_thread("usb", &uhci->super));
}

static void controller_port_configuration(_UHCI *uhci) {
  __UHC_MEMORY__(uhci, m);
  __FOR_RANGE__(port, int, 0, 2) {
    uint16_t status;
    
    if ((status = 
      __STRUCT_CALL__(__CAST__(__UHC__*, uhci), reset_port, port + 1)) > 0) {
        __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), 
        "Port %d enabled ...", port + 1);
#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
      Register *r = uhci->look_for(uhci, Port1_Status);
      r->dump(r, uhci->controller_logger);
#endif
      Register_Type r_type = (port + 1 == 1) ? Port1_Status : Port2_Status;
      Register *port_reg = 
        __STRUCT_CALL__(__CAST__(__UHC__*, uhci), look_up, r_type);

      uint16_t port_bits = *((uint16_t *)port_reg->raw_data);
      if (port_bits & CONNECT)
        __STRUCT_CALL__(uhci, create_dev, status, port + 1, m);
    }
  }
}

static int controller_initializer(_UHCI* uhci) {
  if(__STRUCT_CALL__(uhci, controller_reset) == -1) return -1;
  if(__STRUCT_CALL__(uhci, controller_configuration) == -1) return -1;
  
  __STRUCT_CALL__(uhci, controller_port_configuration);

  return 1;
}

static int controller_reset(_UHCI* uhci) {
  uint32_t read_buffer;

  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), "Performing Global Reset ...");
  // 50ms delay
  __FOR_RANGE__(i, int, 0, 5){
    __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Usb_Command, GRESET);
    mdelay(USB_TDRST);
#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
    reg->dump(reg, uhci->controller_logger);
#endif
    __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Usb_Command, 0);
#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
    reg->dump(reg, uhci->controller_logger);
#endif
  }

  mdelay(USB_TRSTRCY);

  __REGISTER_READ(__CAST__(__UHC__*, uhci), Usb_Command, &read_buffer);

  if (__CAST__(uint16_t, read_buffer) != 0x0000)
    goto fail_label;
  __REGISTER_READ(__CAST__(__UHC__*, uhci), Usb_Status, &read_buffer);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif
  // HC Halted
  if (__CAST__(uint16_t, read_buffer) != HALTED)
    goto fail_label;

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Usb_Status, 0x00FF);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif
  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), "Performing Hardware Reset ...");

  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Usb_Command, HCRESET);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  mdelay(10);

  __REGISTER_READ(__CAST__(__UHC__*, uhci), Usb_Command, &read_buffer);

  if (__CAST__(uint16_t,read_buffer) & HCRESET)
    goto fail_label;
  return 1;

  fail_label:
  __UHC_CALL_LOGGER_ERROR__(__UHC_CAST__(uhci), "Failed to reset UHCI ...");
  return -1;
}

static int controller_configuration(_UHCI *uhci) {
  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), "Configuring UHCI ...");

  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Frame_List_Base_Address, 
      uhci->fba);
  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Frame_Number, 0);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif
  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Start_of_Frame, SOF_FLAG);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif
  uint16_t wVal;
#ifdef UHCI_POLL
  wVal = 0x0000;
  uhci->controller_logger->info_c(uhci->controller_logger,
                                  "Interrupts disabled ...");
#else
  wVal = TMOUT_CRC | COMPLETE;
  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), "Interrupts enabled ...");

#endif
  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Usb_Interrupt, wVal);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Usb_Status, 0xFFFF);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), "Configured UHCI ...");

  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), Usb_Command, MAXP | RS | CF);
  
  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), "UHCI is running ...");

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif
  __STRUCT_CALL__(uhci, controller_port_configuration);
  return 1;
}

static void create_dev(_UHCI *uhci, int16_t status, int pn, MemoryService_C *m) {
  uint8_t speed = (status & LOW_SPEED_ATTACH) ? LOW_SPEED : FULL_SPEED;

  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), 
    "%s-Usb-Device detected at port : %d -> Start configuration",
      speed == FULL_SPEED ? "Full-Speed" : "Low-Speed", pn);
  __DEFAULT_DEV__(speed, pn, __UHC_CAST__(uhci), m);
                      
#if defined(DEVICE_DEBUG_ON) || defined(DEBUG_ON)
  dev->dump_device(dev);
#endif
  if (dev->error_while_transfering) {
    __UHC_CALL_LOGGER_ERROR__(__UHC_CAST__(uhci), 
      "Aborting configuration of Usb-Device on port : %d", pn);
    return;
  }
  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), 
      "Succesful configured Usb-Device on port : %d , "
      "Device : %s, %s",
      pn, dev->manufacturer, dev->product);
}

static void dump_uhci_entry(_UHCI *uhci) {
  Logger_C *controller_logger = __UHC_GET__(uhci, controller_logger);
  PciDevice_Struct *pci_device = __UHC_GET__(uhci, pci_device);
  
  char *msg = "UHCI found : Bus = %d, device = %d, function = %d, vendorID = "
              "0x%x, deviceID = 0x%x, IRQ = %d";

  controller_logger->info_c(controller_logger, msg,
                            pci_device->get_bus_c(pci_device),
                            pci_device->get_device_c(pci_device),
                            pci_device->get_function_c(pci_device),
                            pci_device->get_vendor_id_c(pci_device),
                            pci_device->get_device_id_c(pci_device),
                            pci_device->get_interrupt_line_c(pci_device));
}

static UsbControllerType is_of_type_uhci(UsbController *controller) {
  return TYPE_UHCI;
}

static void init_maps(_UHCI *uhci, MemoryService_C *m) {
  __NEW__(m, QH_TD_Map, sizeof(QH_TD_Map), qh_td_map, new_map, newQH_TD, "Map<QH*,TD*>");
  __NEW__(m, QH_Callback_Function_Map, sizeof(QH_Callback_Function_Map), qh_callback_map,
    new_map,  newQH_CallbackMap, "Map<QH*,callback_function>");
  __NEW__(m, QH_Data_Map, sizeof(QH_Data_Map), qh_data_map, new_map, newQH_Data_Map,
    "Map<QH*,void*>");
  __NEW__(m, QH_Device_Map, sizeof(QH_Device_Map), qh_device_map, new_map, 
    newQH_Device_Map, "Map<QH*,UsbDev*>");  
  __NEW__(m, QH_Device_Request_Map, sizeof(QH_Device_Request_Map), device_request_map, 
    new_map, newQH_DeviceRequest_Map, "Map<QH*,UsbDeviceRequest*>");
  __NEW__(m, QH_Interface_Map, sizeof(QH_Interface_Map), qh_interface_map, new_map,
    newQH_Interface_Map, "Map<QH*, Interface*>");

  uhci->qh_to_td_map = (SuperMap *)qh_td_map;
  uhci->callback_map = (SuperMap *)qh_callback_map;
  uhci->qh_data_map = (SuperMap *)qh_data_map;
  uhci->qh_dev_map = (SuperMap *)qh_device_map;
  uhci->qh_device_request_map = (SuperMap *)device_request_map;
  uhci->qh_interface_map = (SuperMap*)qh_interface_map;

#if defined(TRANSFER_MEASURE_ON)
  __NEW__(m, QH_Measurement_Map, sizeof(QH_Measurement_Map), qh_measurement, new_map,
    newQH_Measuremnt_Map, "Map<QH*,uint32_t*>");
  uhci->qh_measurement = (SuperMap *)qh_measurement;
#endif
}

static uint16_t uhci_reset_port(UsbController *controller, uint8_t port) {
  uint32_t read_buffer;
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);

  Register_Type r = (port == 1 ? Port1_Status : Port2_Status);

  __UHC_CALL_LOGGER_INFO__(controller, 
    "Performing Reset on port = %u", port);
  __REGISTER_READ(__CAST__(__UHC__*, uhci), r, &read_buffer);
  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), r, read_buffer | RESET);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  mdelay(USB_TDRSTR);

  __REGISTER_READ(__CAST__(__UHC__*, uhci), r, &read_buffer);
  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), r, read_buffer & 0xFCB1);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  udelay(300);

  __REGISTER_READ(__CAST__(__UHC__*, uhci), r, &read_buffer);
  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), r, read_buffer | CON_CHANGE | CONNECT);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), r, read_buffer | CONNECT | ENA);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  udelay(50);

  __REGISTER_READ(__CAST__(__UHC__*, uhci), r, &read_buffer);
  __REGISTER_WRITE(__CAST__(__UHC__*, uhci), r, read_buffer | 0x000F);

#if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
#endif

  mdelay(50);

  __REGISTER_READ(__CAST__(__UHC__*, uhci), r, &read_buffer);

  return (__CAST__(uint16_t, read_buffer) & ENA);
}

static QH *request_frames(_UHCI *uhci) {
  QH *current;
  QH *child;
  int map_io_offset = 0;

  __UHC_MEMORY__(uhci, m);

  uint8_t* map_io_buffer = __MAP_IO_KERNEL__(m, uint8_t, 1);
  QH** physical_addresses = 
    __ALLOC_KERNEL_MEM__(m, QH*, SKELETON_SIZE * sizeof(uint32_t *));
  uint32_t* frame_list_address = __MAP_IO_KERNEL__(m, uint32_t, 
    (sizeof(uint32_t) * TOTAL_FRAMES) / PAGE_SIZE);

  // build bulk qh
  QH* __QH_ASSIGN__(map_io_buffer, map_io_offset, bulk_qh);
  __BULK_DEFAULT__(uhci, bulk_qh);
  __ADD_VIRT__(m, bulk_qh, bulk_physical_address);

  // build control qh
  QH* __QH_ASSIGN__(map_io_buffer, map_io_offset, control_qh);
  __CTL_DEFAULT__(uhci, control_qh, bulk_physical_address);
  __ADD_VIRT__(m, control_qh, control_physical_address);

  bulk_qh->parent = control_physical_address;

  __FOR_RANGE__(frame_number, int, 0, TOTAL_FRAMES) {
    __FOR_RANGE_DEC__(j, int, SKELETON_SIZE - 1, 0, 1){
      if ((frame_number + 1) % FRAME_SCHEDULE.qh[j] == 0) {
        if (frame_number + 1 == FRAME_SCHEDULE.qh[j]) {
          __QH_ASSIGN_DEFAULT__(map_io_buffer, map_io_offset, current);

          __STRUCT_CALL__(uhci, __save_address, m, physical_addresses, current, j);

          if (j == 0) {
            __STRUCT_CALL__(uhci, __frame_entry, m, control_qh, current, 
              control_physical_address, frame_list_address, frame_number);
          } 
          else {
            __STRUCT_CALL__(uhci, __frame_entry, m, child, current, 
              __PTR_TYPE__(uint32_t, physical_addresses[j - 1]), 
              frame_list_address, frame_number);
          }
          __INT_DEFAULT__(uhci, current);
          child = current;
          break;
        }
        __STRUCT_CALL__(uhci, __add_to_frame, frame_list_address, 
          frame_number, physical_addresses[j]);
        break;
      }
    }
  }
  __FREE_KERNEL_MEM__(m, physical_addresses);
  __STRUCT_CALL__(uhci, __assign_fba, m, frame_list_address);

  return current;
}

static Addr_Region *i_o_space_layout_run(UsbController* controller, PciDevice_Struct* pci_device) {
  uint16_t command;
  uint16_t base_address;

  _UHCI* uhci = container_of(controller, _UHCI, super);
  __UHC_MEMORY__(uhci, m);

  command = __STRUCT_CALL__(pci_device, readWord_c, COMMAND);
  command &= (0xFFFF ^ (INTERRUPT_DISABLE | MEMORY_SPACE));
  command |= BUS_MASTER | IO_SPACE;
  __STRUCT_CALL__(pci_device, writeWord_c, COMMAND, command);

  base_address = 
    __STRUCT_CALL__(pci_device, readDoubleWord_c, BASE_ADDRESS_4) & 0xFFFFFFFC;;
  
  __UHC_SET__(controller, irq, pci_device->readByte_c(pci_device, INTERRUPT_LINE)); 
  __STRUCT_CALL__(pci_device, writeDoubleWord_c, CAPABILITIES_POINTER, 0x00000000);
  __STRUCT_CALL__(pci_device, writeDoubleWord_c, 0x38, 0x00000000);
  __STRUCT_CALL__(pci_device, writeWord_c, 0xC0, 0x8F00);

  __NEW__(m, IO_Port_Struct_C, sizeof(IO_Port_Struct_C), io_port, newIO_Port,
    newIO_Port, base_address);
  __NEW__(m, IO_Region, sizeof(IO_Region), io_region, new_io_region, new_io_region,
    io_port, 8);

  return (Addr_Region *)io_region;
}

static Register **request_register(UsbController* controller, Addr_Region* addr_region) {
  _UHCI* uhci = container_of(controller, _UHCI, super);
  __UHC_MEMORY__(uhci, m);
  Addr_Region *ar = __UHC_GET__(uhci, addr_region);

  __NEW__(m, Command_Register, sizeof(Command_Register), c_reg, 
          new_command_register, new_command_reg, ar, 
          __ALLOC_KERNEL_MEM__(m, uint16_t, sizeof(uint16_t)));
  __NEW__(m, Status_Register, sizeof(Status_Register), s_reg,
          new_status_register, new_status_reg, ar,
          __ALLOC_KERNEL_MEM__(m, uint16_t, sizeof(uint16_t)));
  __NEW__(m, Interrupt_Register, sizeof(Interrupt_Register), i_reg,
          new_interrupt_register, new_interrupt_reg, ar, 
          __ALLOC_KERNEL_MEM__(m, uint16_t, sizeof(uint16_t)));
  __NEW__(m, Port_Register, sizeof(Port_Register), p_reg_1,
          new_port_register, new_port_reg, 1, ar,
          __ALLOC_KERNEL_MEM__(m, uint16_t, sizeof(uint16_t)));
  __NEW__(m, Port_Register, sizeof(Port_Register), p_reg_2,
          new_port_register, new_port_reg, 2, ar,
          __ALLOC_KERNEL_MEM__(m, uint16_t, sizeof(uint16_t)));
  __NEW__(m, Frame_Numb_Register, sizeof(Frame_Numb_Register), f_n_reg, 
          new_frame_number_register, new_frame_number_reg, ar,
          __ALLOC_KERNEL_MEM__(m, uint16_t, sizeof(uint16_t)));
  __NEW__(m, Frame_Base_Register, sizeof(Frame_Base_Register), f_b_reg,
          new_frame_base_register, new_frame_base_reg, ar,
          __ALLOC_KERNEL_MEM__(m, uint32_t, sizeof(uint32_t)));
  __NEW__(m, SOF_Register, sizeof(SOF_Register), sof_reg,
          new_sof_register, new_sof_reg, ar,
          __ALLOC_KERNEL_MEM__(m, uint8_t, sizeof(uint8_t)));

  Register **regs = 
    __ALLOC_KERNEL_MEM__(m, Register*, sizeof(Register*)*8);
  __ARR_ENTRY__(regs, 0, (Register *)c_reg); 
  __ARR_ENTRY__(regs, 1, (Register *)s_reg); 
  __ARR_ENTRY__(regs, 2, (Register *)i_reg); 
  __ARR_ENTRY__(regs, 3, (Register *)f_n_reg); 
  __ARR_ENTRY__(regs, 4, (Register *)f_b_reg); 
  __ARR_ENTRY__(regs, 5, (Register *)sof_reg); 
  __ARR_ENTRY__(regs, 6, (Register *)p_reg_1); 
  __ARR_ENTRY__(regs, 7, (Register *)p_reg_2);

  return regs;
}

/*Addr_Region *memory_space_layout_run(_UHCI_Builder* builder, PciDevice_Struct*
pci_device){ Memory_Region *addr_region; uint16_t command; uint32_t bar_4, size;
    char *v_addr;
    char *p_addr;
    uint32_t stored_bar;

    MemoryService_C* m = (MemoryService_C*)container_of(builder->mem_service,
MemoryService_C, super);

    command = pci_device->readWord_c(pci_device, COMMAND);
    command |= BUS_MASTER | MEMORY_SPACE;
    pci_device->writeWord_c(pci_device, COMMAND, command);

    stored_bar = pci_device->readDoubleWord_c(pci_device, BASE_ADDRESS_4);

    addr_region = m->allocateKernelMemory_c(m, sizeof(Memory_Region), 0);
    pci_device->writeDoubleWord_c(pci_device, BASE_ADDRESS_4, 0xFFFFFFFF);
    bar_4 = pci_device->readDoubleWord_c(pci_device, BASE_ADDRESS_4) &
0xFFFFFFF0; size = (~(bar_4)) + 1;

    v_addr = (char *)m->mapIO(m, size, 1);

    p_addr = (char *)m->getPhysicalAddress(m, v_addr);
    bar_4 = ((uint32_t)(uintptr_t)p_addr) | (stored_bar & 0x0000000F);
    pci_device->writeDoubleWord_c(pci_device, BASE_ADDRESS_4, bar_4);

    addr_region->new_memory_region = &new_mem_region;

    addr_region->new_memory_region(addr_region,v_addr, size);

    return (Addr_Region*)addr_region;
}*/

static QH *get_free_qh(_UHCI *uhci, uint32_t* qh_id) {
  __GET_FREE_STRUCTURE__(QH, PAGE_SIZE, uhci->map_io_buffer_qh, 
    uhci->map_io_buffer_bit_map_qh, uhci, qh_id);
}

static TD *get_free_td(_UHCI *uhci, uint32_t* td_id) {
  __GET_FREE_STRUCTURE__(TD, (2 * PAGE_SIZE), uhci->map_io_buffer_td,
    uhci->map_io_buffer_bit_map_td, uhci, td_id);
}

static void free_qh(_UHCI *uhci, QH *qh) {
  __FREE_STRUCTURE__(QH, PAGE_SIZE, uhci->map_io_buffer_qh, 
    uhci->map_io_buffer_bit_map_qh, qh, uhci);
}

static void free_td(_UHCI *uhci, TD *td) {
  __FREE_STRUCTURE__(TD, (2*PAGE_SIZE), uhci->map_io_buffer_td, 
    uhci->map_io_buffer_bit_map_td, td, uhci);
}

static void insert_queue(_UHCI *uhci, QH *new_qh, uint16_t priority, enum QH_HEADS v) {
  __UHC_MEMORY__(uhci, m);

  QH* current = __STRUCT_CALL__(uhci, __search_for_qh, m, uhci->qh_entry, v);

  // @deprecated __STRUCT_CALL__(uhci, __qh_inc_device_count, current);

  current = __STRUCT_CALL__(uhci, __follow_schedule, m, current, priority,
    new_qh->flags & QH_FLAG_TYPE_MASK);

  __STRUCT_CALL__(uhci, __adjust_last_in_schedule, current, new_qh);
  __STRUCT_CALL__(uhci,__adjust_last_in_sub_schedule, m, current, new_qh);
  
  new_qh->parent = __PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, current));
  new_qh->flags |= QH_FLAG_IS_QH | QH_FLAG_IN | priority;

  __STRUCT_CALL__(uhci, __add_to_skeleton, m, current, new_qh);
  
  __UHC_RELEASE_LOCK__(uhci);
}

static void remove_queue(_UHCI *uhci, QH *qh) {
  __UHC_MEMORY__(uhci, memory_service);
  __UHC_ACQUIRE_LOCK__(uhci);

  QH *parent = __GET_VIRTUAL__(memory_service, qh->parent, QH);
 
  QH *child = __GET_VIRTUAL__(memory_service, qh->pyhsicalQHLP & QH_ADDRESS_MASK, QH);

  parent->pyhsicalQHLP = qh->pyhsicalQHLP;

  // QH* mqh = __STRUCT_CALL__(uhci, __retrieve_mqh, memory_service, qh);
  // @deprecated __STRUCT_CALL__(uhci, __qh_dec_device_count, mqh);
  __STRUCT_CALL__(uhci, __adjust_remove, qh, parent, child);

  __UHC_RELEASE_LOCK__(uhci);
}

static int remove_transfer_uhci(UsbController* controller, uint32_t transfer_id){
  _UHCI* uhci = (_UHCI*)container_of(controller, _UHCI, super);
  
  QH* qh = __find_qh_in_skeleton_by_id(uhci, transfer_id);
  __IF_RET_ZERO__(__IS_NULL__(qh));
  uhci->remove_queue(uhci, qh); 
  return __RET_S__;
}

static int reset_transfer_uhci(UsbController* controller, uint32_t transfer_id){
  _UHCI* uhci = (_UHCI*)container_of(controller, _UHCI, super);
  QH* qh = __find_qh_in_skeleton_by_id(uhci, transfer_id);
  __IF_RET_ZERO__(__IS_NULL__(qh));
  return __STRUCT_CALL__(uhci, retransmission, qh);
}

static int fast_buffer_change_uhci(struct UsbController* controller, UsbDev* dev, 
  Endpoint* endpoint, uint32_t qh_id, void* buffer){
  __MEM_SERVICE__(controller->mem_service, m);
  _UHCI* uhci = (_UHCI*)container_of(controller, _UHCI, super);
  QH* qh = __STRUCT_CALL__(uhci, __find_qh_in_skeleton_by_id, qh_id);
  __IF_RET_ZERO__(__IS_NULL__(qh));
  uint16_t max_payload = __STRUCT_CALL__(dev, __max_payload, endpoint);
  uint8_t* start = (uint8_t*)buffer;
  TD* td = __MAP_GET__(uhci->qh_to_td_map, TD*, qh);
  while(__NOT_NULL__(td)){
    __STRUCT_CALL__(uhci, __switch_buffer, start, td);
    start += max_payload;
    td = (TD*)__STRUCT_CALL__(m, getVirtualAddressTD, __LP_ADDR__(td));
  }

  return __RET_S__;
}

/*Interface* lock_interface(_UHCI* uhci, UsbDev* dev, unsigned int
interface_num){ return dev->usb_dev_interface_lock(dev, interface_num);
}*/
/* @deprecated moved into device layer
static void free_interface(_UHCI *uhci, UsbDev *dev, Interface *interface) {
  __STRUCT_CALL__(dev, usb_dev_free_interface, interface);
} */

static int uhci_contain_interface(UsbController *controller, Interface *interface) {
  return __IF_EXT__(__STRUCT_CALL__(controller->interface_dev_map, get_c, interface) == 0,
    -1, 1);
}

// replace each method with prototyp (..., UsbDev*, Interface*, ...) -> (...,
// Interface*, ...);
// entry point for usb driver
static uint32_t init_control_transfer(UsbController *controller, Interface *interface,
                           unsigned int pipe, uint8_t priority, void *data,
                           uint8_t *setup, callback_function callback) {
  __TRANSFER_INITIALIZER__(controller, support_control, interface, priority, callback);
  return __STRUCT_CALL__(dev, usb_dev_control, interface, pipe, (uint16_t)shifted_prio,
    data, setup, callback, 0);
}

static uint32_t init_interrupt_transfer(UsbController *controller, Interface *interface,
                             unsigned int pipe, uint8_t priority, void *data,
                             unsigned int len, uint16_t interval,
                             callback_function callback) {
  int16_t mqh;
  __TRANSFER_INITIALIZER__(controller, support_interrupt, interface, priority, callback);
  __IF_CUSTOM__(__NEG_CHECK__((mqh = __STRUCT_CALL__(uhci, is_valid_interval, dev, 
    interval))), callback(dev, interface, E_INVALID_INTERVAL, data); return __RET_N__);
  return __STRUCT_CALL__(dev, usb_dev_interrupt, interface, pipe, (uint16_t)shifted_prio,
    data, len, (uint8_t)mqh, callback);
}

static int16_t is_valid_interval(_UHCI *uhci, UsbDev *dev, uint16_t interval) {
  uint8_t mqh;
  if (interval == interval_1024_ms)
    mqh = QH_1024;
  else if (interval == interval_512_ms)
    mqh = QH_512;
  else if (interval == interval_256_ms)
    mqh = QH_256;
  else if (interval == interval_128_ms)
    mqh = QH_128;
  else if (interval == interval_64_ms)
    mqh = QH_64;
  else if (interval == interval_32_ms)
    mqh = QH_32;
  else if (interval == interval_16_ms)
    mqh = QH_16;
  else if (interval == interval_8_ms)
    mqh = QH_8;
  else if (interval == interval_4_ms)
    mqh = QH_4;
  else if (interval == interval_2_ms)
    mqh = QH_2;
  else if (interval == interval_1_ms)
    mqh = QH_1;
  else {
    __IF_RET_NEG__(interval <= 0);

    interval = __floor_address(interval);
    return __STRUCT_CALL__(uhci, is_valid_interval, dev, interval);
  }
  return mqh;
}

static int16_t is_valid_priority(_UHCI *uhci, UsbDev *dev, uint8_t priority) {
  int16_t shift_prio = -1;
  if (priority == PRIORITY_1)
    shift_prio = PRIORITY_QH_1;
  else if (priority == PRIORITY_2)
    shift_prio = PRIORITY_QH_2;
  else if (priority == PRIORITY_3)
    shift_prio = PRIORITY_QH_3;
  else if (priority == PRIORITY_4)
    shift_prio = PRIORITY_QH_4;
  else if (priority == PRIORITY_5)
    shift_prio = PRIORITY_QH_5;
  else if (priority == PRIORITY_6)
    shift_prio = PRIORITY_QH_6;
  else if (priority == PRIORITY_7)
    shift_prio = PRIORITY_QH_7;
  else if (PRIORITY_8)
    shift_prio = PRIORITY_QH_8;
  return shift_prio;
}

static uint32_t init_bulk_transfer(UsbController *controller, Interface *interface,
                        unsigned int pipe, uint8_t priority, void *data,
                        unsigned int len, callback_function callback) {
  __TRANSFER_INITIALIZER__(controller, support_bulk, interface, priority, callback);
  return __STRUCT_CALL__(dev, usb_dev_bulk, interface, pipe, (uint16_t)shifted_prio, data,
    len, callback, 0);
}

static uint32_t init_iso_transfer(UsbController* controller, Interface* interface,
  unsigned int pipe, uint8_t priority, void* data, unsigned int len, 
  uint16_t interval, callback_function callback) {
  uint16_t mqh;
  __TRANSFER_INITIALIZER__(controller, support_isochronous, interface, priority, callback);
  __IF_CUSTOM__(__NEG_CHECK__((mqh = __STRUCT_CALL__(uhci, is_valid_interval, dev, 
    interval))), callback(dev, interface, E_INVALID_INTERVAL, data); return __RET_N__);
  return __STRUCT_CALL__(dev, usb_dev_iso, interface, pipe, (uint16_t)shifted_prio,
    data, len, (uint8_t)mqh, callback);
}

static uint32_t bulk_entry_point_uhci(UsbDev *dev, Interface* interface, 
                           Endpoint *endpoint, void *data,
                           unsigned int len, uint8_t priority,
                           callback_function callback, uint8_t flags) {
  __UHCI_CONTAINER__(__CAST__(UsbController*, dev->controller), uhci_rcvry);
  return uhci_rcvry->bulk_transfer(uhci_rcvry, dev, data, len, priority, interface, endpoint,
    callback, flags);
}

static uint32_t control_entry_point_uhci(UsbDev *dev, UsbDeviceRequest *device_request,
                              void *data, uint8_t priority, Interface* interface, 
                              Endpoint *endpoint, callback_function callback, 
                              uint8_t flags) {
  __UHCI_CONTAINER__(__CAST__(UsbController*, dev->controller), uhci_rcvry);
  return uhci_rcvry->control_transfer(uhci_rcvry, dev, device_request, data, priority,
                               interface, endpoint, callback, flags);
}

static uint32_t interrupt_entry_point_uhci(UsbDev *dev, Interface* interface, 
                                Endpoint *endpoint, void *data,
                                unsigned int len, uint8_t priority,
                                uint16_t interval, callback_function callback) {
  __UHCI_CONTAINER__(__CAST__(UsbController*, dev->controller), uhci_rcvry);
  return uhci_rcvry->interrupt_transfer(uhci_rcvry, dev, data, len, interval, priority,
                                 interface, endpoint, callback);
}

static uint32_t iso_entry_point_uhci(UsbDev* dev, Interface* interface, 
  Endpoint* endpoint, void* data, 
  unsigned int len, uint8_t priority, uint16_t interval, callback_function callback) {
  __UHCI_CONTAINER__(__CAST__(UsbController*, dev->controller), uhci_rcvry);
  return uhci_rcvry->iso_transfer(uhci_rcvry, dev, data, len, interval, priority, 
    interface, endpoint, callback, 0);
}

// moved into device layer
/* @deprecated static void use_alternate_setting(_UHCI *uhci, UsbDev *dev, Interface *interface,
                           unsigned int setting) {
  dev->request_switch_alternate_setting(dev, interface, setting);
}

static void switch_configuration(_UHCI *uhci, UsbDev *dev, int configuration,
                          callback_function callback) {
  dev->request_switch_configuration(dev, configuration, callback);
} */

static UsbTransfer *build_other_type_transfer(_UHCI *uhci, UsbDev *dev, void *data,
                                     Endpoint *e, unsigned int len,
                                     const char *type, uint8_t flags) {
  unsigned int count = 0;
  
  __UHC_MEMORY__(uhci, m);
  UsbTransfer *usb_transfer = 
    __ALLOC_KERNEL_MEM__(m, UsbTransfer, sizeof(UsbTransfer));
  
  UsbTransaction* first_data_transaction = 
    __STRUCT_CALL__(uhci, build_data_stage_only, data, len, e, dev, &count, flags);
  
  usb_transfer->entry_transaction = first_data_transaction;
  usb_transfer->transaction_count = count;
  usb_transfer->transfer_type = type;
  return usb_transfer;
}

static UsbTransfer *build_control(_UHCI *uhci, UsbDev *dev,
                           UsbDeviceRequest *device_request, void *data,
                           Endpoint *e, uint8_t flags) {
  UsbTransaction *prev_transaction = 0;
  
  unsigned int count = 0;
  
  __UHC_MEMORY__(uhci, m);
  UsbTransfer *usb_transfer = 
    __ALLOC_KERNEL_MEM__(m, UsbTransfer, sizeof(UsbTransfer));

  uint8_t endpoint = __STRUCT_CALL__(dev, __endpoint_default_or_not, e);
  uint8_t packet_type = __STRUCT_CALL__(uhci, __packet_type_control, dev, device_request);
  uint16_t total_bytes_to_transfer = device_request->wLength;

  prev_transaction = __STRUCT_CALL__(uhci, build_setup_stage, device_request, 
    dev, endpoint, flags);
  usb_transfer->entry_transaction = prev_transaction;
  count++;

  prev_transaction = __STRUCT_CALL__(uhci, build_data_stage, dev, total_bytes_to_transfer, 
    packet_type, data, endpoint, prev_transaction, &count, flags);

  __STRUCT_CALL__(uhci, build_status_stage, packet_type, endpoint, dev, prev_transaction);
  
  count++;
  
  usb_transfer->transaction_count = count;
  usb_transfer->transfer_type = CONTROL_TRANSFER;
  return usb_transfer;
}

static UsbTransaction* build_setup_stage(_UHCI* uhci, UsbDeviceRequest* device_request, 
  UsbDev* dev, uint8_t endpoint, uint8_t flags) {
  TokenValues token;
  uint16_t payload_size = 8;
  // setup transaction
  __UHC_MEMORY__(uhci, m);
  
  UsbTransaction *setup_transaction =
      __ALLOC_KERNEL_MEM__(m, UsbTransaction, sizeof(UsbTransaction));
  setup_transaction->transaction_type = SETUP_TRANSACTION;
  token = __STRUCT_CALL__(uhci, __build_token, payload_size, 0, endpoint, 
    dev->address, SETUP);
  setup_transaction->entry_packet = __STRUCT_CALL__(uhci, create_USB_Packet, 
    dev, 0, token, dev->speed, device_request, 0, flags);

  return setup_transaction;
}

static UsbTransaction* build_data_stage(_UHCI* uhci, UsbDev* dev, uint16_t total_bytes_to_transfer,
  uint8_t packet_type, void* data, uint8_t endpoint, UsbTransaction* prev_trans,
  unsigned int *count, uint8_t flags) {
  UsbTransaction *data_transaction = 0;
  uint8_t toggle = 0;
  uint16_t max_len = dev->max_packet_size;
  
  // data transaction
  uint8_t *start = (uint8_t *)data;
  uint8_t *end = start + total_bytes_to_transfer;
  while (start < end) { // run through data and send max payload for endpoint
    toggle ^= 1;
    data_transaction = 
      __STRUCT_CALL__(uhci, build_data_transaction, total_bytes_to_transfer, 
      packet_type, start, end,max_len, dev, endpoint, prev_trans->entry_packet, 
      flags, toggle);
    (*count)++;
    start += max_len;

    prev_trans->next = data_transaction;
    prev_trans = data_transaction;
  }

  return prev_trans;
}

static UsbTransaction* build_data_stage_only(_UHCI* uhci, void* data, unsigned int len,
  Endpoint* e, UsbDev* dev, unsigned int* count, uint8_t flags){
  UsbTransaction *prev_transaction = 0;
  UsbTransaction* head = 0;
  UsbPacket* prev = 0;

  uint16_t max_len = __STRUCT_CALL__(dev, __max_payload, e);
  uint8_t endpoint = __STRUCT_CALL__(dev, __endpoint_number, e);
  uint8_t packet_type = __STRUCT_CALL__(uhci, __packet_type, dev, e);

  uint8_t *start = (uint8_t *)data;
  uint8_t *end = start + len;
  uint8_t toggle = 0;

  while(start < end) { // run through data and send max payload for endpoint
    UsbTransaction* data_transaction = __STRUCT_CALL__(uhci, 
      build_data_transaction, len, packet_type, start, end, 
      max_len, dev, endpoint, prev, flags, toggle);
    if((flags & QH_FLAG_TYPE_MASK) != QH_FLAG_TYPE_ISO)
      toggle ^= 1;
    (*count)++;
    start += max_len;
    if (prev_transaction == (void *)0) {
      head = data_transaction;
    } 
    else {
      prev_transaction->next = data_transaction;
    }
    prev_transaction = data_transaction;
    prev = data_transaction->entry_packet;
  }

  return head;
}

static UsbTransaction* build_data_transaction(_UHCI* uhci, unsigned int len,
  uint8_t packet_type, uint8_t* start, uint8_t* end, uint16_t max_len, UsbDev* dev,
  uint8_t endpoint, UsbPacket* prev, uint8_t flags, uint8_t toggle) {
  TokenValues token;
  UsbTransaction *data_transaction;
  int last_packet = 0;

  __UHC_MEMORY__(uhci, m);
  data_transaction = __ALLOC_KERNEL_MEM__(m, UsbTransaction, sizeof(UsbTransaction));
  __STRUCT_CALL__(uhci, __set_transaction_type, data_transaction, packet_type);
  if(endpoint != 0){
    last_packet = __STRUCT_CALL__(uhci, __is_last_packet, start, end, max_len);
  }
  if (start + max_len > end) { // send less than maximum payload
    max_len = end - start;
  }
  token = __STRUCT_CALL__(uhci, __build_token, max_len, toggle, endpoint, 
    dev->address, packet_type);
  prev = __STRUCT_CALL__(uhci, create_USB_Packet, dev, prev, token, dev->speed, 
    start, last_packet, flags);
  
  data_transaction->entry_packet = prev;
  data_transaction->next = 0;

  return data_transaction;
}

static void build_status_stage(_UHCI* uhci, uint8_t packet_type, uint8_t endpoint,
  UsbDev* dev, UsbTransaction* prev_trans) {
  // status transaction
  // inverted packettype of data stage , empty data packet
  TokenValues token;
  packet_type = __IF_EXT__((packet_type == IN), OUT, IN);

  __UHC_MEMORY__(uhci, m);
  UsbTransaction *status_transaction =
      __ALLOC_KERNEL_MEM__(m, UsbTransaction, sizeof(UsbTransaction));

  token = __STRUCT_CALL__(uhci, __build_token, 0, 1, endpoint, dev->address, 
    packet_type);
  
  status_transaction->transaction_type = STATUS_TRANSACTION;
  
  status_transaction->next = 0;
  status_transaction->entry_packet = __STRUCT_CALL__(uhci, create_USB_Packet, dev, 
    prev_trans->entry_packet, token, dev->speed, 0, 1, 0);
  prev_trans->next = status_transaction;
}

// rename to create Transaction
static UsbPacket *create_USB_Packet(_UHCI *uhci, UsbDev *dev, UsbPacket *prev,
                             TokenValues token, int8_t speed, void *data,
                             int last_packet, uint8_t flags) {
  __UHC_MEMORY__(uhci, m);

  TD* td = __STRUCT_CALL__(uhci, __build_td, dev, prev, &token, speed, data, 
    last_packet, flags);

  UsbPacket *packet = __ALLOC_KERNEL_MEM__(m, UsbPacket, sizeof(UsbPacket));
  packet->internalTD = td;

  __ADD_VIRT_TD__(m, td, phy_td);

#if defined(DEBUG_ON) || defined(TD_DEBUG_ON)
  uhci->inspect_TD(uhci, td);
#endif

  return packet;
}

static void print_USB_Transfer(_UHCI *uhci, UsbTransfer *transfer) {
  int order = 0;
  char *message =
      "Starting with %s-transfer ... \nProcessing %d Transactions ... \n";

  UsbTransaction *transaction = transfer->entry_transaction;

  while (transaction != (void *)0) {
    uhci->print_USB_Transaction(uhci, transaction, order);
    order++;
    transaction = transaction->next;
  }
  message = "%s-transfer finished ... \n";
  __UHC_CALL_LOGGER_DEBUG__(__UHC_CAST__(uhci), message,
                                   transfer->transfer_type);
}

static void print_USB_Transaction(_UHCI *uhci, UsbTransaction *transaction,
                           int order) {
  __UHC_MEMORY__(uhci, m);
  char *message = "Transaction :\nType : %s\nOrder : %d\n"
                  "Packet : %u\n\n"; // packet contains the address of the td
  uint32_t address = (uint32_t)(uintptr_t)(m->getPhysicalAddress(
      m, transaction->entry_packet));
  __UHC_CALL_LOGGER_DEBUG__(__UHC_CAST__(uhci), message,
                            transaction->transaction_type, order,
                            address);
}

static void inspect_QH(_UHCI *uhci, struct QH *qh) {
  __UHC_MEMORY__(uhci, m);
  char *message = "QH Address : %u\n"
                  "QH QHLP : %u\n\tAddress Part : %u\n\tQH/TD Select : "
                  "%u\n\tQH Terminate "
                  ": %u\n"
                  "QH QELP : %u\n\tAddress Part : %u\n\tQH/TD Select : "
                  "%u\n\tTD Terminate "
                  ": %u\n"
                  "QH Parent : %u\n"
                  "QH Flags : \n\tEND = %x , IN = %x\n"
                  "\tPRIORITY = %x\n\tMQH = %x , QH = %x\n"
                  "\tTYPE = %x\n\tQH_ID = %u\n\n";

  uint32_t address = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, qh));
  __UHC_CALL_LOGGER_DEBUG__(__UHC_CAST__(uhci), message, address, qh->pyhsicalQHLP,
      qh->pyhsicalQHLP & QH_ADDRESS_MASK,
      (qh->pyhsicalQHLP & QH_TD_SELECT) >> 1, qh->pyhsicalQHLP & QH_TERMINATE,
      qh->pyhsicalQHEP, qh->pyhsicalQHEP & QH_ADDRESS_MASK,
      (qh->pyhsicalQHEP & QH_TD_SELECT) >> 1, qh->pyhsicalQHEP & QH_TERMINATE,
      qh->parent, qh->flags & QH_FLAG_END, !(qh->flags & QH_FLAG_END),
      (qh->flags & PRIORITY_QH_MASK) >> 1, (qh->flags & QH_FLAG_IS_MQH) >> 4,
      !((qh->flags & QH_FLAG_IS_MQH) >> 4),
      (qh->flags & QH_FLAG_TYPE_MASK) >> 5,
      (qh->flags & QH_ID_MASK));
}

static void inspect_TD(_UHCI *uhci, struct TD *td) {
  __UHC_MEMORY__(uhci, m);
  char *message =
      "TD Address : %u\n"
      "TD LP : %u\n\tAddress Part : %u\n\tDEPTH Select : %u\n\tQH/TD Select "
      ": "
      "%u\n\tTD Terminate : %u\n"
      "TD Control & Status :\n\tC_ERR : %u\n\tLS : %u\n\tIOC : %u\n\tActive "
      ": "
      "%u\n"
      "\tStalled : %u\n\tNAK : %u\n\tCRC : %u\n\tActual Length : %u\n"
      "TD Token :\n\tMax Length : %u\n\tData Toggle : %u\n\tEndpoint : "
      "%u\n\t"
      "Device Address : %u\n\tPID : %u\n"
      "Buffer Pointer : %u\n\n";

  uint32_t address = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, td));
  __UHC_CALL_LOGGER_DEBUG__(__UHC_CAST__(uhci), message, address, td->pyhsicalLinkPointer,
      td->pyhsicalLinkPointer & QH_ADDRESS_MASK,
      (td->pyhsicalLinkPointer & DEPTH_BREADTH_SELECT) >> 2,
      (td->pyhsicalLinkPointer & QH_TD_SELECT) >> 1,
      td->pyhsicalLinkPointer & QH_TERMINATE,
      (td->control_x_status >> C_ERR) & 0x03,
      (td->control_x_status >> LS) & 0x01, (td->control_x_status >> IOC) & 0x01,
      (td->control_x_status >> ACTIVE) & 0x01,
      (td->control_x_status >> STALLED) & 0x01,
      (td->control_x_status >> NAK_RECV) & 0x01,
      (td->control_x_status >> CRC) & 0x01,
      (td->control_x_status & ACTUAL_LENGTH_MASK),
      (td->token & TD_MAX_LENGTH_MASK) >> TD_MAX_LENGTH,
      (td->token >> TD_DATA_TOGGLE) & 0x01, (td->token >> TD_ENDPOINT) & 0xF,
      (td->token >> TD_DEVICE_ADDRESS) & 0x7F, td->token & 0xFF,
      td->bufferPointer);
}

static void inspect_transfer(_UHCI *uhci, QH *qh, TD *td) {
  __UHC_MEMORY__(uhci, memory_service);

  uhci->inspect_QH(uhci, qh);

  while (td != (void *)0) {
    uhci->inspect_TD(uhci, td);
    td = (TD *)memory_service->getVirtualAddressTD(
        memory_service, td->pyhsicalLinkPointer & QH_ADDRESS_MASK);
  }
}

static void dump_skeleton(_UHCI *uhci) {
  QH *qh = uhci->qh_entry;
  __UHC_MEMORY__(uhci, memory_service);

  while (qh != (void *)0) {
    uhci->inspect_QH(uhci, qh);

    qh = (QH *)memory_service->getVirtualAddress(
        memory_service, qh->pyhsicalQHLP & QH_ADDRESS_MASK);
  }
}

static void dump_all(_UHCI *uhci) {
  __UHC_MEMORY__(uhci, mem_service);
  QH *qh = uhci->qh_entry;
  TD *td;

  while (qh != (void *)0) {
    uhci->inspect_QH(uhci, qh);
    td = (TD *)mem_service->getVirtualAddressTD(
        mem_service, qh->pyhsicalQHEP & QH_ADDRESS_MASK);
    while (td != (void *)0) {
      uhci->inspect_TD(uhci, td);
      td = (TD *)mem_service->getVirtualAddressTD(
          mem_service, td->pyhsicalLinkPointer & QH_ADDRESS_MASK);
    }
    qh = (QH *)mem_service->getVirtualAddress(mem_service, qh->pyhsicalQHLP &
                                                               QH_ADDRESS_MASK);
  }
}

// insert flag CONFIGURED : used for not using interrupts instead poll for
// timeout -> like the controls
static uint32_t bulk_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
  uint8_t priority, Interface* interface, Endpoint *e, callback_function callback, uint8_t flags) {
  __UHC_MEMORY__(uhci, m);
  
  uint32_t qh_id;
  QH *qh = __STRUCT_CALL__(uhci, get_free_qh, &qh_id);
  __STRUCT_CALL__(uhci, __default_qh, qh);
  __ADD_VIRT__(m, qh, qh_physical);

  UsbTransfer *transfer = build_other_type_transfer(uhci, dev, data, e, len, 
    BULK_TRANSFER, flags & BULK_INITIAL_STATE);

  TD *td = transfer->entry_transaction->entry_packet->internalTD;

  __STRUCT_CALL__(uhci, __set_flags, qh, QH_FLAG_TYPE_BULK, 
    qh_id, 0);
  __STRUCT_CALL__(uhci, __set_qhep, qh, td, m);

  if (!(flags & BULK_INITIAL_STATE)) 
    __STRUCT_CALL__(uhci, __save_map_properties, qh, td, data, dev, interface, callback);
  
#if defined(DEBUG_ON) || defined(TRANSFER_DEBUG_ON)
  uhci->print_USB_Transfer(uhci, transfer);
  uhci->inspect_transfer(uhci, qh, td);
#endif
  __STRUCT_CALL__(uhci, destroy_transfer, transfer);
  __STRUCT_CALL__(uhci, insert_queue, qh, priority, QH_BULK);

  if ((flags & BULK_INITIAL_STATE) > 0) {
    __STRUCT_CALL__(uhci,__initial_state_routine, UPPER_BOUND_TIME_OUT_MILLIS_BULK, 
      qh, flags, dev, interface, data, qh_physical, td, m, callback);
  }

  return qh_id;
}

static uint32_t interrupt_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
  uint16_t interval, uint8_t priority, Interface* interface, Endpoint *e, callback_function callback) {
  __UHC_MEMORY__(uhci, m);

  uint32_t qh_id;
  QH *qh = __STRUCT_CALL__(uhci, get_free_qh, &qh_id);
  __STRUCT_CALL__(uhci, __default_qh, qh);
  __ADD_VIRT__(m, qh, qh_physical);

  UsbTransfer *transfer = build_other_type_transfer(uhci, dev, data, e, len, 
    INTERRUPT_TRANSFER, 0);

  TD *td = transfer->entry_transaction->entry_packet->internalTD;

  __STRUCT_CALL__(uhci, __set_qhep, qh, td, m);
  __STRUCT_CALL__(uhci, __set_flags, qh, QH_FLAG_TYPE_INTERRUPT, 
    qh_id, 0);
  __STRUCT_CALL__(uhci, __save_map_properties, qh, td, data, dev, interface, callback);

#if defined(DEBUG_ON) || defined(TRANSFER_DEBUG_ON)
  uhci->print_USB_Transfer(uhci, transfer);
  uhci->inspect_transfer(uhci, qh, td);
#endif

  __STRUCT_CALL__(uhci, destroy_transfer, transfer);
  __STRUCT_CALL__(uhci, insert_queue, qh, priority, (enum QH_HEADS)interval);

  return qh_id;
}

static uint32_t iso_transfer(_UHCI* uhci, UsbDev* dev, void* data, unsigned int len,
  uint16_t interval, uint8_t priority, Interface* interface, Endpoint *e, 
  callback_function callback, uint16_t flags){
  __UHC_MEMORY__(uhci, m);

  uint32_t qh_id;
  QH *qh = __STRUCT_CALL__(uhci, get_free_qh, &qh_id);
  __STRUCT_CALL__(uhci, __default_qh, qh);
  __ADD_VIRT__(m, qh, qh_physical);

  UsbTransfer *transfer = build_other_type_transfer(uhci, dev, data, e, len, 
    ISO_TRANSFER, flags | QH_FLAG_TYPE_ISO);

  TD *td = transfer->entry_transaction->entry_packet->internalTD;

  __STRUCT_CALL__(uhci, __set_qhep, qh, td, m);
  __STRUCT_CALL__(uhci, __set_flags, qh, QH_FLAG_TYPE_ISO, 
    qh_id, (flags & ISO_EXT) ? QH_FLAG_ISO_EXT : 0);
  __STRUCT_CALL__(uhci, __save_map_properties, qh, td, data, dev, interface, callback);

#if defined(DEBUG_ON) || defined(TRANSFER_DEBUG_ON)
  uhci->print_USB_Transfer(uhci, transfer);
  uhci->inspect_transfer(uhci, qh, td);
#endif

  __STRUCT_CALL__(uhci, destroy_transfer, transfer);
  __STRUCT_CALL__(uhci, insert_queue, qh, priority, (enum QH_HEADS)interval);

  return qh_id;
}

static uint32_t iso_transfer_ext(UsbController* controller, Interface* interface, 
  Endpoint* e, void* data, unsigned int len,
  uint16_t interval, uint8_t priority, callback_function callback){
  _UHCI* uhci = (_UHCI*)container_of(controller, _UHCI, super);
  UsbDev* dev = ((UsbController*)uhci)->interface_dev_map->get_c(
    ((UsbController*)uhci)->interface_dev_map, interface);
  uint16_t shifted_prio = (uint16_t)is_valid_priority(uhci, dev, priority);
  uint16_t shifted_interval =  (uint16_t)is_valid_interval(uhci, dev, interval);

  return __STRUCT_CALL__(uhci, iso_transfer, dev, data, len, shifted_interval, shifted_prio,
    interface, e, callback, ISO_EXT);
}

static uint32_t control_transfer(_UHCI *uhci, UsbDev *dev, UsbDeviceRequest *rq,
                      void *data, uint8_t priority, Interface* interface, Endpoint *endpoint,
                      callback_function callback, uint8_t flags) {
  __UHC_MEMORY__(uhci, m);

  uint32_t qh_id;
  QH* qh = __STRUCT_CALL__(uhci, get_free_qh, &qh_id);
  __STRUCT_CALL__(uhci, __default_qh, qh);
  __ADD_VIRT__(m, qh, qh_physical);

  UsbTransfer *transfer = build_control(uhci, dev, rq, data, endpoint, 
    flags & CONTROL_INITIAL_STATE);

  TD *internalTD = transfer->entry_transaction->entry_packet->internalTD;
  __STRUCT_CALL__(uhci, __set_qhep, qh, internalTD, m);
  __STRUCT_CALL__(uhci, __set_flags, qh, QH_FLAG_TYPE_CONTROL, 
    qh_id, 0);

  __STRUCT_CALL__(uhci, destroy_transfer, transfer);

  if ((dev->state == CONFIGURED_STATE) && !(flags & CONTROL_INITIAL_STATE)) 
    __STRUCT_CALL__(uhci, __save_map_properties_control, qh, internalTD,
       data, dev, interface, callback, rq);
  
  __STRUCT_CALL__(uhci, insert_queue, qh, priority, QH_CTL);

  if (dev->state != CONFIGURED_STATE || ((flags & CONTROL_INITIAL_STATE) > 0)) 
    __STRUCT_CALL__(uhci,__initial_state_routine_control, 
      UPPER_BOUND_TIME_OUT_MILLIS_CONTROL, qh, flags, dev, interface, data, 
      qh_physical, internalTD, m, rq, callback);

  return qh_id;
}

// wait time out time -> upper bound 10ms
static uint32_t wait_poll(_UHCI *uhci, QH *process_qh, uint32_t timeout, uint8_t flags) {
  TD *td;
  uint32_t current_time;

  __UHC_MEMORY__(uhci, m);
  uint32_t status = E_TRANSFER;
  uint32_t initial_time = getSystemTimeInMilli();
  uint32_t time_out_time = addMilis(initial_time, timeout);

  do {
    current_time = getSystemTimeInMilli();
    td = (TD*)__STRUCT_CALL__(m, getVirtualAddressTD, __QHEP_ADDR__(process_qh));
    __IF_SINGLE_BREAK__(__IS_NULL__(td), status = S_TRANSFER); // if null -> transmission was successful
  } while (current_time < time_out_time);

  __IF_NOT_NULL__(td) { // transfer not sucessful
    status |= __STRUCT_CALL__(uhci, get_status, td);
    if(flags != SUPRESS_DEVICE_ERRORS)
      __UHC_CALL_LOGGER_ERROR__(__UHC_CAST__(uhci), 
      "transfer was not sucessful ! error mask %u",
      status);
  }

#if defined(TRANSFER_MEASURE_ON)
  uint32_t *i_time =
      (uint32_t *)uhci->qh_measurement->get_c(uhci->qh_measurement, process_qh);
  uint32_t transfer_type = process_qh->flags & QH_FLAG_TYPE_MASK;
  uint32_t transfer_duration;
  const char *measure_msg;
  const char *transfer_type_msg;

  if(transfer_type == QH_FLAG_TYPE_BULK)
    transfer_type_msg = BULK_TRANSFER;
  else if(transfer_type == QH_FLAG_TYPE_CONTROL)
    transfer_type_msg = CONTROL_TRANSFER;
#if defined(MEASURE_MS)
  measure_msg = "%s Transfer Duration in ms: %u";
  transfer_duration = getSystemTimeInMilli() - *i_time;
#elif defined(MEASURE_NS)
  measure_msg = "%s Transfer Duration in ns: %u";
  transfer_duration = getSystemTimeInNano() - *i_time;
#elif defined(MEASURE_MCS)
  measure_msg = "%s Transfer Duration in micro: %u";
  transfer_duration = getSystemTimeInMicro() - *i_time;
#endif
  m->freeKernelMemory_c(m, i_time, 0);
  uhci->controller_logger->info_c(uhci->controller_logger, measure_msg,
                                  transfer_type_msg, transfer_duration);
#endif

  return status;
}

static uint32_t get_status(_UHCI *uhci, TD *td) {
  uint32_t status = td->control_x_status;
  uint32_t error_mask = 0;
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
  char *message;
#endif

  if ((status >> NAK_RECV) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "NAK received";
    __UHC_CALL_LOGGER_DEBUG__(((UsbController*)uhci), 
      message);
#endif

    error_mask |= E_NAK_RECEIVED;
  }
  if ((status >> STALLED) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "Stalled received";
    __UHC_CALL_LOGGER_DEBUG__(((UsbController*)uhci), 
      message);
#endif

    error_mask |= E_STALLED;
  }
  if ((status >> DATA_BUFFER_ERROR) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "Data Buffer Error received";
    __UHC_CALL_LOGGER_DEBUG__(((UsbController*)uhci), 
      message);
#endif

    error_mask |= E_DATA_BUFFER;
  }
  if ((status >> BABBLE_DETECTED) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "Babble Detected received";
    __UHC_CALL_LOGGER_DEBUG__(((UsbController*)uhci), 
      message);
#endif

    error_mask |= E_BABBLE_DETECTED;
  }
  if ((status >> CRC) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "CRC Error received";
    __UHC_CALL_LOGGER_DEBUG__(((UsbController*)uhci), 
      message);
#endif

    error_mask |= E_CRC;
  }
  if ((status >> BITSTUFF_ERROR) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "Bitstuff Error received";
    __UHC_CALL_LOGGER_DEBUG__(((UsbController*)uhci), 
      message);;
#endif

    error_mask |= E_BITSTUFF;
  }

  return error_mask;
}

// if process_qh is null -> qh got already processed -> check if
// we need retransmit it : meaning check if it is a Interrupt Transfer
// if not null -> check retransmission current tries and resend the QH

// return if retransmission occured or not !
static unsigned int retransmission(_UHCI *uhci, QH *process_qh) {
  TD *saved_td = 0;
  TD *head = 0;

  unsigned int retransmission_occured = 0;
  __UHC_MEMORY__(uhci, m);
  uint8_t transfer_type = process_qh->flags & QH_FLAG_TYPE_MASK;
  if (transfer_type == QH_FLAG_TYPE_INTERRUPT) {
    saved_td = (TD*)__STRUCT_CALL__(uhci->qh_to_td_map, get_c, process_qh);
    head = saved_td;
    while (__NOT_NULL__(saved_td)) {
      saved_td->control_x_status = ((0x1 << LS) & saved_td->control_x_status) |
                                   ((0x1 << IOC) & saved_td->control_x_status) |
                                   (0x3 << C_ERR) | (0x1 << ACTIVE);
      saved_td = (TD*)__STRUCT_CALL__(m, getVirtualAddressTD, 
        __LP_ADDR__(saved_td));
    }
  __STRUCT_CALL__(uhci, __set_qhep, process_qh, head, m);
    retransmission_occured = 1;
  }
  else if(transfer_type == QH_FLAG_TYPE_ISO) {
    saved_td = (TD*)__STRUCT_CALL__(uhci->qh_to_td_map, get_c, process_qh);
    head = saved_td;
    while (__NOT_NULL__(saved_td)) {
      saved_td->control_x_status = ((0x1 << LS) & saved_td->control_x_status) |
                                   ((0x1 << IOC) & saved_td->control_x_status) |
                                   ((0x1 << IOS) & saved_td->control_x_status) |
                                   (0x3 << C_ERR) | (0x1 << ACTIVE);
      saved_td = (TD*)__STRUCT_CALL__(m, getVirtualAddressTD, 
        __LP_ADDR__(saved_td));
    }
  __STRUCT_CALL__(uhci, __set_qhep, process_qh, head, m);
    retransmission_occured = 1;
  }
  return retransmission_occured;
}

static void remove_td_linkage(_UHCI *uhci, TD *start) {
  TD *temp;
  __UHC_MEMORY__(uhci, m);
  
  while (__NOT_NULL__(start)) {
    uint32_t p_addr = __PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, start));
    temp = (TD*)__STRUCT_CALL__(m, getVirtualAddressTD, __LP_ADDR__(start)); 
    __STRUCT_CALL__(uhci, free_td, start);
    __STRUCT_CALL__(m, remove_virtualAddressTD, p_addr);
    start = temp;
  }
}

static void _poll_uhci_(UsbController *controller) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);
  __UHC_MEMORY__(uhci, m);
  QH *entry = uhci->qh_entry;
  for (;;) {
    if (__IS_NULL__(entry)) {
      entry = uhci->qh_entry;
    }
    __STRUCT_CALL__(uhci, traverse_skeleton, entry);
    entry = __GET_VIRTUAL__(m, __QHLP_ADDR__(entry), QH);
  }
}

static void transmission_clearing_routine(_UHCI* uhci, QH* entry, 
  MemoryService_C* mem_service) {
  TD *rcvry = __MAP_GET__(uhci->qh_to_td_map, TD*, entry);
  __STRUCT_CALL__(uhci, remove_queue, entry);
  __STRUCT_CALL__(uhci, remove_transfer_entry, entry);
  uint32_t phy_qh =__PTR_TYPE__(uint32_t,
    __GET_PHYSICAL__(mem_service, entry));
  __STRUCT_CALL__(mem_service, remove_virtualAddress, phy_qh);
  __STRUCT_CALL__(uhci, free_qh, entry);
  __STRUCT_CALL__(uhci, remove_td_linkage, rcvry);
}

static void successful_transmission_routine(_UHCI* uhci, callback_function callback, 
  UsbDev* dev, Interface* interface, void* data, QH* entry, MemoryService_C* mem_service) {
  unsigned int retransmission_occured;
  callback(dev, interface, S_TRANSFER, data);
  retransmission_occured = __STRUCT_CALL__(uhci, retransmission, entry);
  if (!retransmission_occured) {
    __STRUCT_CALL__(uhci, transmission_clearing_routine, entry, mem_service);
  }
}

static void failed_transmission_routine(_UHCI* uhci, UsbDev* dev, Interface* interface, 
  void* data, TD* td, QH* entry, MemoryService_C* mem_service, callback_function callback) {
  uint32_t error_mask = __STRUCT_CALL__(uhci, get_status, td);
  
  __IF_RET__(!error_mask || ((entry->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_ISO));
  // if switching to the zero bandwidth setting in iso transfers, the transfer should still reside in the
  // schedule, but shouldn't be executed -> goal is when activated to refresh TD's correctly, so
  // that they can be executed
  /*__IF_SINGLE_RET__(((entry->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_ISO),
    retransmission(uhci, entry)); //this approach is 100% safe, because the transfer is always ready to go*/

  __STRUCT_CALL__(uhci, transmission_clearing_routine, entry, mem_service);

  callback(dev, interface, E_TRANSFER | error_mask, data);
}

static void traverse_skeleton(_UHCI *uhci, QH *entry) {
  // uhci->inspect_QH(uhci, entry);
  __IF_RET__(__IS_NULL__(entry));
  __IF_RET__((entry->flags & QH_FLAG_IS_MASK) == QH_FLAG_IS_MQH);

  __UHC_MEMORY__(uhci, mem_service);

  __IF_RET__((entry->flags & QH_FLAG_ISO_EXT));

  TD *td = (TD *)__STRUCT_CALL__(mem_service, getVirtualAddressTD, 
    __QHEP_ADDR__(entry));

  callback_function callback = 
    __MAP_GET__(uhci->callback_map, callback_function, entry);
  void *data = __MAP_GET__(uhci->qh_data_map, void*, entry);
  UsbDev *dev = __MAP_GET__(uhci->qh_dev_map, UsbDev*, entry);
  Interface* interface = __MAP_GET__(uhci->qh_interface_map, Interface*, entry);

  if (__IS_NULL__(td)) { // transmission successful
    // uhci->inspect_QH(uhci, entry);
    // uhci->inspect_TD(uhci, td);

    __STRUCT_CALL__(uhci, successful_transmission_routine, callback, dev, interface,
      data, entry, mem_service);
  }
  else if (!__STRUCT_CALL__(uhci, __td_failed, td)) {
    __STRUCT_CALL__(uhci, failed_transmission_routine, dev, interface, data, td, entry,
      mem_service, callback);
  }
}

// remove all map entries + free memory
static void remove_transfer_entry(_UHCI *uhci, QH *entry) {
  if ((entry->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_CONTROL) {
    UsbDev *dev = uhci->qh_dev_map->get_c(uhci->qh_dev_map, entry);
    UsbDeviceRequest *device_req =
        (UsbDeviceRequest *)uhci->qh_device_request_map->get_c(
            uhci->qh_device_request_map, entry);
    dev->free_device_request(dev, device_req);

    uhci->qh_device_request_map->remove_c(uhci->qh_device_request_map, entry);
  }

  uhci->qh_to_td_map->remove_c(uhci->qh_to_td_map, entry);
  uhci->callback_map->remove_c(uhci->callback_map, entry);
  uhci->qh_data_map->remove_c(uhci->qh_data_map, entry);
  uhci->qh_dev_map->remove_c(uhci->qh_dev_map, entry);
  uhci->qh_interface_map->remove_c(uhci->qh_interface_map, entry);
}

static void destroy_transfer(_UHCI *uhci, UsbTransfer *transfer) {
  __UHC_MEMORY__(uhci, m);

  UsbTransaction *transaction = transfer->entry_transaction;

  while (transaction != (void *)0) {
    UsbPacket *packet = transaction->entry_packet;
    UsbTransaction *temp = transaction->next;
    m->freeKernelMemory_c(m, packet, 0);
    m->freeKernelMemory_c(m, transaction, 0);

    transaction = temp;
  }

  m->freeKernelMemory_c(m, transfer, 0);
}

static void runnable_function_uhci(UsbController *controller) {
  _UHCI *uhci = container_of(controller, _UHCI, super);
  __UHC_MEMORY__(uhci, m);
  //QH* periodic_first_mqh = __STRUCT_CALL__(uhci, __search_for_qh, m, uhci->qh_entry, QH_1);
  for (;;) {
    if (uhci->signal) {
      __FOR_RANGE_COND__(qh, QH*, uhci->qh_entry, qh != 0, 
        qh = __GET_VIRTUAL__(m, __QHLP_ADDR__(qh), QH)) {
        uhci->traverse_skeleton(uhci, qh);
      }
      __IF_ELSE__(!uhci->signal_not_override, uhci->signal = 0, 
        uhci->signal_not_override = 0);
    } 
    else {
      /*QH* current = __GET_VIRTUAL__(m, __QHLP_ADDR__(periodic_first_mqh), QH);
      while((current->flags & QH_FLAG_TYPE_MASK) != QH_FLAG_TYPE_CONTROL){
        TD *td = (TD *)__STRUCT_CALL__(m, getVirtualAddressTD, 
        __QHEP_ADDR__(current));

        callback_function callback = 
          __MAP_GET__(uhci->callback_map, callback_function, current);
        void *data = __MAP_GET__(uhci->qh_data_map, void*, current);
        UsbDev *dev = __MAP_GET__(uhci->qh_dev_map, UsbDev*, current);
        Interface* interface = __MAP_GET__(uhci->qh_interface_map, Interface*, current);

        if (__IS_NULL__(td)) { // transmission successful
          __STRUCT_CALL__(uhci, successful_transmission_routine, callback, dev, interface,
            data, current, m);
        }
        current = __GET_VIRTUAL__(m, __QHLP_ADDR__(current), QH);
      } */
      yield_c();
    }
  }
}

static void handler_function_uhci(UsbController *controller) {
  _UHCI *uhci = container_of(controller, _UHCI, super);
  if (uhci->signal)
    uhci->signal_not_override = 1;
  uhci->signal = 1;
  // stop interrupt -> write clear
  Register *s_reg =
    __STRUCT_CALL__(__CAST__(__UHC__*, uhci), look_up, Usb_Status);

  uint16_t lval = INT | ERR_INT;

  __STRUCT_CALL__(s_reg, write, &lval);
}