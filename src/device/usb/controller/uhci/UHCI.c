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
static void bulk_entry_point_uhci(struct UsbDev *dev, Endpoint *endpoint, void *data,
                      unsigned int len, uint8_t priority,
                      callback_function callback, uint8_t flags);
static void control_entry_point_uhci(struct UsbDev *dev,
                         struct UsbDeviceRequest *device_request, void *data,
                         uint8_t priority, Endpoint *endpoint,
                         callback_function callback, uint8_t flags);
static void interrupt_entry_point_uhci(struct UsbDev *dev, Endpoint *endpoint, void *data,
                           unsigned int len, uint8_t priority,
                           uint16_t interval, callback_function callback);
static void insert_queue(struct _UHCI *uhci, struct QH *new_qh,
                  uint16_t priority, enum QH_HEADS v);
static void remove_queue(struct _UHCI *uhci, struct QH *qh);
static void control_transfer(_UHCI *uhci, UsbDev *dev, struct UsbDeviceRequest *rq,
                      void *data, uint8_t priority, Endpoint *endpoint,
                      build_control_transfer build_function,
                      callback_function callback, uint8_t flags);
static void interrupt_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                        uint16_t interval, uint8_t priority, Endpoint *e,
                        build_bulk_or_interrupt_transfer build_function,
                        callback_function callback);
static void bulk_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                   uint8_t, Endpoint *e, build_bulk_or_interrupt_transfer build_function,
                   callback_function callback, uint8_t flags);
static uint32_t wait_poll(_UHCI *uhci, QH *process_qh, uint32_t timeout, uint8_t flags);
static void _poll_uhci_(UsbController *controller);
static QH *get_free_qh(_UHCI *uhci);
static TD *get_free_td(_UHCI *uhci);
static void free_qh(_UHCI *uhci, QH *qh);
static void free_td(_UHCI *uhci, TD *td);
static void init_control_transfer(UsbController *controller, Interface *interface,
                           unsigned int pipe, uint8_t priority, void *data,
                           uint8_t *setup, callback_function callback);
static void init_interrupt_transfer(UsbController *controller, Interface *interface,
                             unsigned int pipe, uint8_t priority, void *data,
                             unsigned int len, uint16_t interval,
                             callback_function callback);
static void init_bulk_transfer(UsbController *controller, Interface *interface,
                        unsigned int pipe, uint8_t priority, void *data,
                        unsigned len, callback_function callback);
static unsigned int retransmission(_UHCI *uhci, struct QH *process_qh);
static void traverse_skeleton(_UHCI *uhci, struct QH *entry);
static uint32_t get_status(_UHCI *uhci, TD *td);
static UsbPacket *create_USB_Packet(_UHCI *uhci, UsbDev *dev, UsbPacket *prev,
                             struct TokenValues token, int8_t speed, void *data,
                             int last_packet, uint8_t flags);
static UsbTransfer *build_control(_UHCI *uhci, UsbDev *dev,
                           UsbDeviceRequest *device_request, void *data,
                           Endpoint *e, uint8_t flags);
static UsbTransfer *build_interrupt_or_bulk(_UHCI *uhci, UsbDev *dev, void *data,
                                     Endpoint *e, unsigned int len,
                                     const char *type, uint8_t flags);
static int uhci_contain_interface(UsbController *controller, Interface *interface);
static int16_t is_valid_priority(_UHCI *uhci, UsbDev *dev, uint8_t priority,
                          callback_function callback);
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
static int16_t is_valid_interval(_UHCI* uhci, UsbDev* dev, uint16_t interval, void* data);
static int controller_initializer(_UHCI* uhci);
static int controller_reset(_UHCI* uhci);

static inline void __assign_fba(MemoryService_C* m, 
                                uint32_t* frame_list, _UHCI* uhci) {
    uhci->fba = 
        __PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, frame_list));
}   

static inline void __init_buff_mem(_UHCI* uhci, uint32_t qh_size, uint32_t td_size) {
    __mem_set(uhci->map_io_buffer_qh, qh_size, 0); 
    __mem_set(uhci->map_io_buffer_td, td_size, 0); 
    __mem_set(uhci->map_io_buffer_bit_map_qh, qh_size / sizeof(QH), 0);
    __mem_set(uhci->map_io_buffer_bit_map_td, td_size / sizeof(TD), 0);
}

static inline void __uhci_build(_UHCI* uhci, MemoryService_C* m, PciDevice_Struct* pci_device,
                                uint32_t qh_size, uint32_t td_size) {
    __INIT_UHCI__(uhci, (SystemService_C*)m, pci_device);
    uhci->qh_len = qh_size;
    uhci->td_len = td_size;
    uhci->map_io_buffer_qh = __MAP_IO_KERNEL__(m, uint8_t, qh_size);
    uhci->map_io_buffer_bit_map_qh = __ALLOC_KERNEL_MEM__(m, uint8_t, qh_size / sizeof(QH));
    
    uhci->map_io_buffer_td = __MAP_IO_KERNEL__(m, uint8_t, td_size);
    uhci->map_io_buffer_bit_map_td = __ALLOC_KERNEL_MEM__(m, uint8_t, td_size / sizeof(TD));
    
    __init_buff_mem(uhci, qh_size, td_size);

    uhci->signal = 0;
    uhci->signal_not_override = 0;
}

static inline void __save_address(MemoryService_C* m, QH** physical_addr, QH* current, 
                     int pos) {
    uint32_t physical;
    QH* qh__ = (QH*)__GET_PHYSICAL__(m, current);
    __ARR_ENTRY__(physical_addr, pos, qh__);
    physical = __PTR_TYPE__(uint32_t, physical_addr[pos]);
    __STRUCT_CALL__(m, addVirtualAddress, physical, current);
}

static inline void __frame_entry(MemoryService_C* m, QH* next, QH* current, 
                                 uint32_t next_physical_addr, 
                                 uint32_t* frame_list, int fn) {
    current->pyhsicalQHLP = next_physical_addr | QH_SELECT;
    next->parent = __PTR_TYPE__(uint32_t, 
        __GET_PHYSICAL__(m, current));
    frame_list[fn] = __PTR_TYPE__(uint32_t, 
        __GET_PHYSICAL__(m, current)) | QH_SELECT;
}

static inline void __build_qh(QH* qh, uint32_t flags, uint32_t qhlp,
                              uint32_t qhep, uint32_t parent) {
    qh->flags = flags;
    qh->pyhsicalQHLP = qhlp;
    qh->pyhsicalQHEP = qhep;
    qh->parent = parent;
}

static inline void __add_to_frame(uint32_t* frame_list, int fn,
                                  QH* physical) {
    frame_list[fn] = 
        (__PTR_TYPE__(uint32_t, physical)) | QH_SELECT;
}

static inline void __qh_set_parent(MemoryService_C* m, QH* qh, QH* parent) {
    qh->parent = __PTR_TYPE__(uint32_t, __GET_PHYSICAL__(m, parent));
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

  uhci->dump_uhci_entry(uhci);

  uhci->qh_entry = uhci->request_frames(uhci);

  uhci->init_maps(uhci, m);
  __add_look_up_registers(__UHC_CAST__(uhci), 8);

  if (uhci->controller_configuration(uhci))
    create_thread("usb", &uhci->super);
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
  uhci->controller_port_configuration(uhci);
  return 1;
}

static void create_dev(_UHCI *uhci, int16_t status, int pn, MemoryService_C *m) {
  uint8_t speed = (status & LOW_SPEED_ATTACH) ? LOW_SPEED : FULL_SPEED;

  __UHC_CALL_LOGGER_INFO__(__UHC_CAST__(uhci), 
    "%s-Usb-Device detected at port : %d -> Start configuration",
      speed == FULL_SPEED ? "Full-Speed" : "Low-Speed", pn);
  __NEW__(m, UsbDev, sizeof(UsbDev), dev, new_usb_device, 
    new_usb_device, speed, pn, 0, 0xFF, pn, pn, (SystemService_C *)m,
                      uhci, 0);
                      
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

  uhci->qh_to_td_map = (SuperMap *)qh_td_map;
  uhci->callback_map = (SuperMap *)qh_callback_map;
  uhci->qh_data_map = (SuperMap *)qh_data_map;
  uhci->qh_dev_map = (SuperMap *)qh_device_map;
  uhci->qh_device_request_map = (SuperMap *)device_request_map;

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

  uint8_t* map_io_buffer = __MAP_IO_KERNEL__(m, uint8_t, PAGE_SIZE);
  QH** physical_addresses = 
    __ALLOC_KERNEL_MEM__(m, QH*, SKELETON_SIZE * sizeof(uint32_t *));
  uint32_t* frame_list_address = __MAP_IO_KERNEL__(m, uint32_t, 
    sizeof(uint32_t) * TOTAL_FRAMES);

  // build bulk qh
  QH* __QH_ASSIGN__(map_io_buffer, map_io_offset, bulk_qh);
  __BULK_DEFAULT__(bulk_qh);
  __ADD_VIRT__(m, bulk_qh, bulk_physical_address);

  // build control qh
  QH* __QH_ASSIGN__(map_io_buffer, map_io_offset, control_qh);
  __CTL_DEFAULT__(control_qh, bulk_physical_address);
  __ADD_VIRT__(m, control_qh, control_physical_address);

  bulk_qh->parent = control_physical_address;

#if defined(DEBUG_ON) || defined(SKELETON_DEBUG_ON)
  uhci->controller_logger->debug_c(
      uhci->controller_logger, "bulk qh address : %d", bulk_physical_address);
  uhci->controller_logger->debug_c(uhci->controller_logger, "bulk qhlp : %d",
                                   bulk_qh->pyhsicalQHLP & QH_ADDRESS_MASK);

  uhci->controller_logger->debug_c(uhci->controller_logger,
                                   "control qh address : %d",
                                   control_physical_address);
  uhci->controller_logger->debug_c(uhci->controller_logger, "control qhlp : %d",
                                   control_qh->pyhsicalQHLP & QH_ADDRESS_MASK);
#endif
  __FOR_RANGE__(frame_number, int, 0, TOTAL_FRAMES) {
    __FOR_RANGE_DEC__(j, int, SKELETON_SIZE - 1, 0, 1){
      if ((frame_number + 1) % FRAME_SCHEDULE.qh[j] == 0) {
        if (frame_number + 1 == FRAME_SCHEDULE.qh[j]) {
          __QH_ASSIGN_DEFAULT__(map_io_buffer, map_io_offset, current);

          __save_address(m, physical_addresses, current, j);

          if (j == 0) {
            __frame_entry(m, control_qh, current, control_physical_address, 
              frame_list_address, frame_number);
          } 
          else {
            __frame_entry(m, child, current, __PTR_TYPE__(uint32_t, physical_addresses[j - 1])
              , frame_list_address, frame_number);
          }
          __INT_DEFAULT__(current);

#if defined(DEBUG_ON) || defined(SKELETON_DEBUG_ON)
          uhci->controller_logger->debug_c(uhci->controller, "address : %d",
                                           physical);
          uhci->controller_logger->debug_c(uhci->controller, "qhlp : %d",
                                           current->pyhsicalQHLP &
                                               QH_ADDRESS_MASK);
#endif

          child = current;
          break;
        }
        __add_to_frame(frame_list_address, frame_number, physical_addresses[j]);
        break;
      }
    }
  }
  __FREE_KERNEL_MEM__(m, physical_addresses);
  __assign_fba(m, frame_list_address, uhci);

  return current;
}

static Addr_Region *i_o_space_layout_run(UsbController* controller, PciDevice_Struct* pci_device) {
  uint16_t command;
  uint16_t base_address;

  _UHCI* uhci = container_of(controller, _UHCI, super);
  __UHC_MEMORY__(uhci, m);

  command = pci_device->readWord_c(pci_device, COMMAND);
  command &= (0xFFFF ^ (INTERRUPT_DISABLE | MEMORY_SPACE));
  command |= BUS_MASTER | IO_SPACE;
  pci_device->writeWord_c(pci_device, COMMAND, command);

  base_address =
      pci_device->readDoubleWord_c(pci_device, BASE_ADDRESS_4) & 0xFFFFFFFC;
  
  __UHC_SET__(controller, irq, pci_device->readByte_c(pci_device, INTERRUPT_LINE)); 

  pci_device->writeDoubleWord_c(pci_device, CAPABILITIES_POINTER, 0x00000000);
  pci_device->writeDoubleWord_c(pci_device, 0x38, 0x00000000);

  pci_device->writeWord_c(pci_device, 0xC0, 0x8F00);

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

  __ADD_REGISTER_ENTRIES__(regs);

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

static QH *get_free_qh(_UHCI *uhci) {
  __FOR_RANGE__(i, int, 0, PAGE_SIZE / sizeof(QH)){
    __USB_LOCK__(__UHC_CAST__(uhci));
    if (uhci->map_io_buffer_bit_map_qh[i] == 0) {
      uhci->map_io_buffer_bit_map_qh[i] = 1;
      __USB_RELEASE__(__UHC_CAST__(uhci));
      return (QH *)(uhci->map_io_buffer_qh + (i * sizeof(QH)));
    }
    __USB_RELEASE__(__UHC_CAST__(uhci));
  }
  return (void *)0;
}

static TD *get_free_td(_UHCI *uhci) {
  for (int i = 0; i < ((2 * PAGE_SIZE) / sizeof(TD)); i++) {
    __USB_LOCK__(__UHC_CAST__(uhci));
    if (uhci->map_io_buffer_bit_map_td[i] == 0) {
      uhci->map_io_buffer_bit_map_td[i] = 1;
      // uhci->mutex->release_c(uhci->mutex);
      return (TD *)(uhci->map_io_buffer_td + (i * sizeof(TD)));
    }
    // uhci->mutex->release_c(uhci->mutex);
  }
  return (void *)0;
}

static void free_qh(_UHCI *uhci, QH *qh) {
  for (int i = 0; i < PAGE_SIZE; i += sizeof(QH)) {
    // uhci->mutex->acquire_c(uhci->mutex);
    if ((uhci->map_io_buffer_qh + i) == (uint8_t *)qh) {
      uhci->map_io_buffer_bit_map_qh[i / sizeof(QH)] = 0;
      // uhci->mutex->release_c(uhci->mutex);
      return;
    }
    // uhci->mutex->release_c(uhci->mutex);
  }
}

static void free_td(_UHCI *uhci, TD *td) {
  for (int i = 0; i < (2 * PAGE_SIZE); i += sizeof(TD)) {
    // uhci->mutex->acquire_c(uhci->mutex);
    if ((uhci->map_io_buffer_td + i) == (uint8_t *)td) {
      uhci->map_io_buffer_bit_map_td[i / sizeof(TD)] = 0;
      // uhci->mutex->release_c(uhci->mutex);
      return;
    }
    // uhci->mutex->release_c(uhci->mutex);
  }
}

static void insert_queue(_UHCI *uhci, QH *new_qh, uint16_t priority, enum QH_HEADS v) {
  __UHC_MEMORY__(uhci, m);

  uint32_t physical_addr;
  int8_t offset = (int8_t)v;
  QH *current = uhci->qh_entry;

  __UHC_ACQUIRE_LOCK__(uhci);

  while (offset > 0) {
    if ((current->flags & QH_FLAG_IS_MQH)) {
      offset--;
    }
    physical_addr = (current->pyhsicalQHLP & QH_ADDRESS_MASK);
    current = m->getVirtualAddress(m, physical_addr);
  }

  current->flags = ((((current->flags & QH_FLAG_DEVICE_COUNT_MASK) >>
                      QH_FLAG_DEVICE_COUNT_SHIFT) +
                     1)
                    << QH_FLAG_DEVICE_COUNT_SHIFT) |
                   (current->flags & 0xFF);

  while (((current->flags & QH_FLAG_END_MASK) == QH_FLAG_IN) &&
         (((current->flags & PRIORITY_QH_MASK) >> 1) >= priority)) {
    current =
        (QH *)m->getVirtualAddress(m, current->pyhsicalQHLP & QH_ADDRESS_MASK);
  }

  if ((current->pyhsicalQHLP & QH_TERMINATE) ==
      QH_TERMINATE) { // erase bit which got set prior
    current->pyhsicalQHLP &= 0xFFFFFFFE;
    new_qh->pyhsicalQHLP = QH_TERMINATE | QH_SELECT;
  } else {
    new_qh->pyhsicalQHLP = current->pyhsicalQHLP;
  }

  if ((current->flags & QH_FLAG_END_MASK) == QH_FLAG_END) {
    new_qh->flags |= QH_FLAG_END;
    current->flags &= 0xFFFFFFFE;
  } else { // if not last element in chain update parent pointer
    QH *c =
        (QH *)m->getVirtualAddress(m, current->pyhsicalQHLP & QH_ADDRESS_MASK);
    c->parent = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, new_qh));
  }
  new_qh->parent = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, current));
  new_qh->flags |= QH_FLAG_IS_QH | QH_FLAG_IN | priority;

#if defined(TRANSFER_MEASURE_ON)
  if ((new_qh->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_BULK ||
      (new_qh->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_CONTROL) {
    uint32_t *measure =
        (uint32_t *)m->allocateKernelMemory_c(m, sizeof(uint32_t), 0);
#if defined(MEASURE_MS)
    *measure = getSystemTimeInMilli();
#elif defined(MEASURE_NS)
    *measure = getSystemTimeInNano();
#elif defined(MEASURE_MCS)
    *measure = getSystemTimeInMicro();
#endif
    uhci->qh_measurement->put_c(uhci->qh_measurement, new_qh, measure);
  }
#endif

  // adds to skeleton -> gets executed by controller !
  current->pyhsicalQHLP =
      ((uint32_t)(uintptr_t)(m->getPhysicalAddress(m, new_qh))) | QH_SELECT;

#if defined(DEBUG_ON) || defined(QH_DEBUG_ON)
  uhci->inspect_QH(uhci, new_qh);
  uhci->inspect_QH(uhci, current);
#endif
  __UHC_RELEASE_LOCK__(uhci);
}

// todo as in the insert queue method
static void remove_queue(_UHCI *uhci, QH *qh) {
  __UHC_MEMORY__(uhci, memory_service);
  __UHC_ACQUIRE_LOCK__(uhci);

  QH *parent =
      (QH *)memory_service->getVirtualAddress(memory_service, qh->parent);
  QH *child = (QH *)memory_service->getVirtualAddress(
      memory_service, qh->pyhsicalQHLP & QH_ADDRESS_MASK);

  parent->pyhsicalQHLP = qh->pyhsicalQHLP;

  QH *mqh = qh;

  while ((mqh->flags & QH_FLAG_IS_MASK) == QH_FLAG_IS_QH) {
    mqh = (QH *)memory_service->getVirtualAddress(memory_service, mqh->parent);
  }

  mqh->flags = ((((mqh->flags & QH_FLAG_DEVICE_COUNT_MASK) >>
                  QH_FLAG_DEVICE_COUNT_SHIFT) -
                 1)
                << QH_FLAG_DEVICE_COUNT_SHIFT) |
               (mqh->flags & 0xFF);

  if ((qh->flags & QH_FLAG_END_MASK) == QH_FLAG_END) {
    parent->flags |= QH_FLAG_END;
  }

  if ((qh->flags & QH_FLAG_END_MASK) ==
      QH_FLAG_IN) { // if qh is not last in chain update child pointer to
    // parent
    child->parent = qh->parent;
  }

  __UHC_RELEASE_LOCK__(uhci);
}

/*Interface* lock_interface(_UHCI* uhci, UsbDev* dev, unsigned int
interface_num){ return dev->usb_dev_interface_lock(dev, interface_num);
}*/

static void free_interface(_UHCI *uhci, UsbDev *dev, Interface *interface) {
  dev->usb_dev_free_interface(dev, interface);
}

static int uhci_contain_interface(UsbController *controller, Interface *interface) {

  return controller->interface_dev_map->get_c(controller->interface_dev_map,
                                              interface) == (void *)0
             ? -1
             : 1;
}

// replace each method with prototyp (..., UsbDev*, Interface*, ...) -> (...,
// Interface*, ...);
// entry point for usb driver
static void init_control_transfer(UsbController *controller, Interface *interface,
                           unsigned int pipe, uint8_t priority, void *data,
                           uint8_t *setup, callback_function callback) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);

  UsbDev *dev = (UsbDev *)controller->interface_dev_map->get_c(
      controller->interface_dev_map, interface);

  if (dev == (void *)0) {
    callback(0, E_INTERFACE_NOT_SUPPORTED, data);
    return;
  }

  if (!dev->support_control(dev, interface)) {
    callback(dev, E_NOT_SUPPORTED_TRANSFER_TYPE, data);
    return;
  }
  int16_t shifted_prio;
  if ((shifted_prio = uhci->is_valid_priority(uhci, dev, priority, callback)) ==
      -1) {
    callback(dev, E_PRIORITY_NOT_SUPPORTED, data);
    return;
  }

  dev->usb_dev_control(dev, interface, pipe, priority, data, setup, callback,
                       0);
}

static void init_interrupt_transfer(UsbController *controller, Interface *interface,
                             unsigned int pipe, uint8_t priority, void *data,
                             unsigned int len, uint16_t interval,
                             callback_function callback) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);

  UsbDev *dev = controller->interface_dev_map->get_c(
      controller->interface_dev_map, interface);

  if (dev == (void *)0) {
    callback(0, E_INTERFACE_INV, data);
    return;
  }

  if (!dev->support_interrupt(dev, interface)) {
    callback(dev, E_NOT_SUPPORTED_TRANSFER_TYPE, data);
    return;
  }
  int16_t shifted_prio;
  if ((shifted_prio = uhci->is_valid_priority(uhci, dev, priority, callback)) ==
      -1) {
    callback(dev, E_PRIORITY_NOT_SUPPORTED, data);
    return;
  }

  int16_t mqh;

  if ((mqh = uhci->is_valid_interval(uhci, dev, interval, data)) == -1) {
    callback(dev, E_INVALID_INTERVAL, data);
    return;
  }

  dev->usb_dev_interrupt(dev, interface, pipe, (uint16_t)shifted_prio, data,
                         len, (uint8_t)mqh, callback);
}

static int16_t is_valid_interval(_UHCI *uhci, UsbDev *dev, uint16_t interval,
                          void *data) {
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
    if (interval <= 0)
      return -1;

    interval = __floor_address(interval);

    return uhci->is_valid_interval(uhci, dev, interval, data);
  }

  return mqh;
}

static int16_t is_valid_priority(_UHCI *uhci, UsbDev *dev, uint8_t priority,
                          callback_function callback) {
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

static void init_bulk_transfer(UsbController *controller, Interface *interface,
                        unsigned int pipe, uint8_t priority, void *data,
                        unsigned len, callback_function callback) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);

  UsbDev *dev = controller->interface_dev_map->get_c(
      controller->interface_dev_map, interface);

  if (dev == (void *)0) {
    callback(0, E_INTERFACE_INV, data);
    return;
  }
  if (!dev->support_bulk(dev, interface)) {
    callback(dev, E_NOT_SUPPORTED_TRANSFER_TYPE, data);
    return;
  }
  int16_t shifted_prio;
  if ((shifted_prio = uhci->is_valid_priority(uhci, dev, priority, callback)) ==
      -1) {
    callback(dev, E_PRIORITY_NOT_SUPPORTED, data);
    return;
  }

  dev->usb_dev_bulk(dev, interface, pipe, (uint16_t)shifted_prio, data, len,
                    callback, 0);
}

static void bulk_entry_point_uhci(UsbDev *dev, Endpoint *endpoint, void *data,
                           unsigned int len, uint8_t priority,
                           callback_function callback, uint8_t flags) {
  _UHCI *uhci_rcvry =
      (_UHCI *)container_of((UsbController *)dev->controller, _UHCI, super);

  uhci_rcvry->bulk_transfer(uhci_rcvry, dev, data, len, priority, endpoint,
                            &build_interrupt_or_bulk, callback, flags);
}

static void control_entry_point_uhci(UsbDev *dev, UsbDeviceRequest *device_request,
                              void *data, uint8_t priority, Endpoint *endpoint,
                              callback_function callback, uint8_t flags) {
  _UHCI *uhci_rcvry =
      (_UHCI *)container_of(((UsbController *)dev->controller), _UHCI, super);

  uhci_rcvry->control_transfer(uhci_rcvry, dev, device_request, data, priority,
                               endpoint, &build_control, callback, flags);
}

static void interrupt_entry_point_uhci(UsbDev *dev, Endpoint *endpoint, void *data,
                                unsigned int len, uint8_t priority,
                                uint16_t interval, callback_function callback) {
  _UHCI *uhci_rcvry =
      container_of((UsbController *)dev->controller, _UHCI, super);

  uhci_rcvry->interrupt_transfer(uhci_rcvry, dev, data, len, interval, priority,
                                 endpoint, &build_interrupt_or_bulk, callback);
}

static void use_alternate_setting(_UHCI *uhci, UsbDev *dev, Interface *interface,
                           unsigned int setting, callback_function callback) {
  dev->request_switch_alternate_setting(dev, interface, setting, callback);
}

static void switch_configuration(_UHCI *uhci, UsbDev *dev, int configuration,
                          callback_function callback) {
  dev->request_switch_configuration(dev, configuration, callback);
}

static UsbTransfer *build_interrupt_or_bulk(_UHCI *uhci, UsbDev *dev, void *data,
                                     Endpoint *e, unsigned int len,
                                     const char *type, uint8_t flags) {
  UsbPacket *prev = 0;
  __UHC_MEMORY__(uhci, m);

  UsbTransaction *prev_transaction = 0;
  UsbTransaction *data_transaction = 0;

  data_transaction->next = 0;

  TokenValues token;
  int count = 0;
  
  UsbTransfer *usb_transfer =
      (UsbTransfer *)m->allocateKernelMemory_c(m, sizeof(UsbTransfer), 0);

  uint8_t toggle = 0;
  uint16_t max_len = e->endpoint_desc.wMaxPacketSize & WMAX_PACKET_SIZE_MASK;
  uint8_t endpoint = e->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;

  // data transaction
  uint8_t *start = (uint8_t *)data;
  uint8_t *end = start + len;
  uint8_t packet_type =
      (e->endpoint_desc.bEndpointAddress & DIRECTION_IN) ? IN : OUT;
  int last_packet = 0;
  while (start < end) { // run through data and send max payload for endpoint
    data_transaction = (UsbTransaction *)m->allocateKernelMemory_c(
        m, sizeof(UsbTransaction), 0);
    data_transaction->transaction_type =
        (packet_type == OUT) ? DATA_OUT_TRANSACTION : DATA_IN_TRANSACTION;
    last_packet = (start + max_len) >= end ? 1 : 0;
    if (start + max_len > end) { // send less than maximum payload
      max_len = end - start;
    }
    token = (TokenValues){.max_len = max_len,
                          .toggle = toggle,
                          .endpoint = endpoint,
                          .address = dev->address,
                          .packet_type = packet_type};
    prev = uhci->create_USB_Packet(uhci, dev, prev, token, dev->speed, start,
                                   last_packet, flags);
    toggle ^= 1;
    count++;
    start += max_len;

    data_transaction->entry_packet = prev;
    data_transaction->next = 0;

    if (prev_transaction == (void *)0) {
      usb_transfer->entry_transaction = data_transaction;
    } else {
      prev_transaction->next = data_transaction;
    }
    prev_transaction = data_transaction;
  }

  usb_transfer->transaction_count = count;
  usb_transfer->transfer_type = type;
  return usb_transfer;
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
                  "\tTYPE = %x\n\tQH_TD = %u , QH_QH = %u\n\n";

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
      (qh->flags & QH_FLAG_IS_MASK) == QH_FLAG_IS_QH
          ? (qh->flags & QH_FLAG_DEVICE_COUNT_MASK) >> 8
          : 0,
      (qh->flags & QH_FLAG_IS_MASK) == QH_FLAG_IS_MQH
          ? (qh->flags & QH_FLAG_DEVICE_COUNT_MASK) >> 8
          : 0);
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

static UsbTransfer *build_control(_UHCI *uhci, UsbDev *dev,
                           UsbDeviceRequest *device_request, void *data,
                           Endpoint *e, uint8_t flags) {
  UsbPacket *head = 0;
  UsbPacket *prev = 0;

  __UHC_MEMORY__(uhci, m);

  UsbTransaction *prev_transaction = 0;
  UsbTransaction *data_transaction = 0;

  TokenValues token;
  unsigned int count = 0;
  unsigned int endpoint =
      (e == ((void *)0) ? 0
                        : e->endpoint_desc.bEndpointAddress & ENDPOINT_MASK);

  UsbTransfer *usb_transfer =
      (UsbTransfer *)m->allocateKernelMemory_c(m, sizeof(UsbTransfer), 0);

  uint8_t toggle = 0;
  uint16_t total_bytes_to_transfer = device_request->wLength;
  uint16_t max_len = 8; // max payload of setup transaction

  // setup transaction
  UsbTransaction *setup_transaction =
      (UsbTransaction *)m->allocateKernelMemory_c(m, sizeof(UsbTransaction), 0);
  usb_transfer->entry_transaction = setup_transaction;
  setup_transaction->transaction_type = SETUP_TRANSACTION;
  token = (TokenValues){.max_len = max_len,
                        .toggle = toggle,
                        .endpoint = endpoint,
                        .address = dev->address,
                        .packet_type = SETUP};
  head = uhci->create_USB_Packet(uhci, dev, 0, token, dev->speed,
                                 device_request, 0, flags);
  setup_transaction->entry_packet = head;
  count++;
  prev = head;
  prev_transaction = setup_transaction;

  max_len = dev->max_packet_size;
  // data transaction
  uint8_t *start = (uint8_t *)data;
  uint8_t *end = start + total_bytes_to_transfer;
  uint8_t packet_type =
      (device_request->bmRequestType & DEVICE_TO_HOST) ? IN : OUT;
  while (start < end) { // run through data and send max payload for endpoint
    data_transaction = (UsbTransaction *)m->allocateKernelMemory_c(
        m, sizeof(UsbTransaction), 0);
    data_transaction->transaction_type =
        (packet_type == OUT) ? DATA_OUT_TRANSACTION : DATA_IN_TRANSACTION;
    toggle ^= 1;
    if (start + max_len > end) { // send less than maximum payload
      max_len = end - start;
    }
    token = (TokenValues){.max_len = max_len,
                          .toggle = toggle,
                          .endpoint = endpoint,
                          .address = dev->address,
                          .packet_type = packet_type};
    prev = uhci->create_USB_Packet(uhci, dev, prev, token, dev->speed, start, 0,
                                   0);
    count++;
    start += max_len;
    data_transaction->entry_packet = prev;
    prev_transaction->next = data_transaction;
    prev_transaction = data_transaction;
  }

  toggle = 1;
  // status transaction
  // inverted packettype of data stage , empty data packet
  packet_type = (packet_type == IN ? OUT : IN);
  UsbTransaction *status_transaction =
      (UsbTransaction *)m->allocateKernelMemory_c(m, sizeof(UsbTransaction), 0);
  token = (TokenValues){.max_len = 0,
                        .toggle = toggle,
                        .endpoint = endpoint,
                        .address = dev->address,
                        .packet_type = packet_type};
  status_transaction->transaction_type = STATUS_TRANSACTION;
  prev = uhci->create_USB_Packet(uhci, dev, prev, token, dev->speed, 0, 1, 0);
  count++;
  status_transaction->entry_packet = prev;
  prev_transaction->next = status_transaction;

  status_transaction->next = 0;

  usb_transfer->transaction_count = count;
  usb_transfer->transfer_type = CONTROL_TRANSFER;
  return usb_transfer;
}

// rename to create Transaction
static UsbPacket *create_USB_Packet(_UHCI *uhci, UsbDev *dev, UsbPacket *prev,
                             TokenValues token, int8_t speed, void *data,
                             int last_packet, uint8_t flags) {
  __UHC_MEMORY__(uhci, m);

  TD *td = uhci->get_free_td(uhci);

  td->bufferPointer = 0;
  td->control_x_status = 0;
  td->pyhsicalLinkPointer = 0;
  td->token = 0;

  if (prev != (void *)0) {
    prev->internalTD->pyhsicalLinkPointer |=
        ((uint32_t)(uintptr_t)(m->getPhysicalAddress(m, td)));
  }

  td->pyhsicalLinkPointer = DEPTH_BREADTH_SELECT | TD_SELECT;

  td->control_x_status = (speed == LOW_SPEED ? (0x1 << LS) : (0x0 << LS));
  td->control_x_status |= (0x1 << ACTIVE);
  td->control_x_status |= (0x3 << C_ERR); // 3 Fehlversuche max

  if (last_packet) {
    td->pyhsicalLinkPointer |= QH_TERMINATE;

    if ((dev->state == CONFIGURED_STATE) && (flags != BULK_INITIAL_STATE) &&
        (flags != CONTROL_INITIAL_STATE)) {
      td->control_x_status |=
          (0x1 << IOC); // set interrupt bit only for last td in qh !
    }
  }

  td->token = ((token.max_len - 1) & 0x7FF) << TD_MAX_LENGTH;
  td->token |= (token.toggle << TD_DATA_TOGGLE);
  td->token |= (token.endpoint << TD_ENDPOINT);
  td->token |= (token.address << TD_DEVICE_ADDRESS);
  td->token |= (token.packet_type);

  td->bufferPointer = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, data));

  UsbPacket *packet =
      (UsbPacket *)m->allocateKernelMemory_c(m, sizeof(UsbPacket), 0);
  packet->internalTD = td;

  uint32_t p = (uint32_t)(uintptr_t)m->getPhysicalAddress(m, td);
  m->addVirtualAddressTD(m, p, td);

#if defined(DEBUG_ON) || defined(TD_DEBUG_ON)
  uhci->inspect_TD(uhci, td);
#endif

  return packet;
}

// insert flag CONFIGURED : used for not using interrupts instead poll for
// timeout -> like the controls
static void bulk_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                   uint8_t priority, Endpoint *e,
                   build_bulk_or_interrupt_transfer build_function,
                   callback_function callback, uint8_t flags) {
  __UHC_MEMORY__(uhci, m);

  QH *qh = uhci->get_free_qh(uhci);

  qh->flags = 0;
  qh->parent = 0;
  qh->pyhsicalQHEP = 0;
  qh->pyhsicalQHLP = 0;

  uint32_t qh_physical = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, qh));
  m->addVirtualAddress(m, qh_physical, qh);

  UsbTransfer *transfer =
      build_function(uhci, dev, data, e, len, BULK_TRANSFER, flags & BULK_INITIAL_STATE);

  TD *td = transfer->entry_transaction->entry_packet->internalTD;

  qh->flags = (transfer->transaction_count << QH_FLAG_DEVICE_COUNT_SHIFT) |
              QH_FLAG_TYPE_BULK;

  qh->pyhsicalQHEP = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, td));

  if (!(flags & BULK_INITIAL_STATE)) {
    uhci->qh_to_td_map->put_c(uhci->qh_to_td_map, qh, td);
    uhci->qh_data_map->put_c(uhci->qh_data_map, qh, data);
    uhci->qh_dev_map->put_c(uhci->qh_dev_map, qh, dev);
    uhci->callback_map->put_c(uhci->callback_map, qh, callback);
  }

#if defined(DEBUG_ON) || defined(TRANSFER_DEBUG_ON)
  uhci->print_USB_Transfer(uhci, transfer);
  uhci->inspect_transfer(uhci, qh, td);
#endif

  uhci->destroy_transfer(uhci, transfer);

  uhci->insert_queue(uhci, qh, priority, QH_BULK);

  if ((flags & BULK_INITIAL_STATE) > 0) {
    uint32_t status =
        uhci->wait_poll(uhci, qh, UPPER_BOUND_TIME_OUT_MILLIS_BULK, 
                        flags & SUPRESS_DEVICE_ERRORS);

    callback(dev, status, data);

    uhci->remove_queue(uhci, qh);
    m->remove_virtualAddress(m, qh_physical);
    uhci->free_qh(uhci, qh);
    uhci->remove_td_linkage(uhci, td);
  }
}

static void interrupt_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                        uint16_t interval, uint8_t priority, Endpoint *e,
                        build_bulk_or_interrupt_transfer build_function,
                        callback_function callback) {
  __UHC_MEMORY__(uhci, m);

  QH *qh = uhci->get_free_qh(uhci);

  qh->flags = 0;
  qh->parent = 0;
  qh->pyhsicalQHEP = 0;
  qh->pyhsicalQHLP = 0;

  uint32_t qh_physical = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, qh));
  m->addVirtualAddress(m, qh_physical, qh);

  UsbTransfer *transfer =
      build_function(uhci, dev, data, e, len, INTERRUPT_TRANSFER, 0);

  TD *td = transfer->entry_transaction->entry_packet->internalTD;

  qh->flags = (transfer->transaction_count << QH_FLAG_DEVICE_COUNT_SHIFT) |
              QH_FLAG_TYPE_INTERRUPT;
  qh->pyhsicalQHEP = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, td));

  uhci->qh_to_td_map->put_c(uhci->qh_to_td_map, qh, td);
  uhci->qh_data_map->put_c(uhci->qh_data_map, qh, data);
  uhci->qh_dev_map->put_c(uhci->qh_dev_map, qh, dev);
  uhci->callback_map->put_c(uhci->callback_map, qh, callback);

#if defined(DEBUG_ON) || defined(TRANSFER_DEBUG_ON)
  uhci->print_USB_Transfer(uhci, transfer);
  uhci->inspect_transfer(uhci, qh, td);
#endif

  uhci->destroy_transfer(uhci, transfer);

  uhci->insert_queue(uhci, qh, priority, (enum QH_HEADS)interval);
}

// wait time out time -> upper bound 10ms
static uint32_t wait_poll(_UHCI *uhci, QH *process_qh, uint32_t timeout, uint8_t flags) {
  TD *td;

  __UHC_MEMORY__(uhci, m);
  uint32_t status = E_TRANSFER;
  uint32_t current_time;
  uint32_t initial_time = getSystemTimeInMilli();
  uint32_t time_out_time = addMilis(initial_time, timeout);

  do {
    current_time = getSystemTimeInMilli();
    td = (TD *)m->getVirtualAddressTD(m, process_qh->pyhsicalQHEP &
                                             QH_ADDRESS_MASK);
    if (td == (void *)0) { // if null -> transmission was successful
      status = S_TRANSFER;
      break;
    }
  } while (current_time < time_out_time);

  if (td != (void *)0) { // transfer not sucessful
    status |= uhci->get_status(uhci, td);
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
  char *message;

  if ((status >> NAK_RECV) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "NAK received";
    uhci->controller_logger->debug_c(uhci->controller_logger, message);
#endif

    error_mask |= E_NAK_RECEIVED;
  }
  if ((status >> STALLED) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "Stalled received";
    uhci->controller_logger->debug_c(uhci->controller_logger, message);
#endif

    error_mask |= E_STALLED;
  }
  if ((status >> DATA_BUFFER_ERROR) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "Data Buffer Error received";
    uhci->controller_logger->debug_c(uhci->controller_logger, message);
#endif

    error_mask |= E_DATA_BUFFER;
  }
  if ((status >> BABBLE_DETECTED) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "Babble Detected received";
    uhci->controller_logger->debug_c(uhci->controller_logger, message);
#endif

    error_mask |= E_BABBLE_DETECTED;
  }
  if ((status >> CRC) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "CRC Error received";
    uhci->controller_logger->debug_c(uhci->controller_logger, message);
#endif

    error_mask |= E_CRC;
  }
  if ((status >> BITSTUFF_ERROR) & 0x01) {
#if defined(DEBUG_ON) || defined(STATUS_DEBUG_ON)
    message = "Bitstuff Error received";
    uhci->controller_logger->debug_c(uhci->controller_logger, message);
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
    saved_td = (TD *)uhci->qh_to_td_map->get_c(uhci->qh_to_td_map, process_qh);
    head = saved_td;
    while (saved_td != (void *)0) {
      saved_td->control_x_status = ((0x1 << LS) & saved_td->control_x_status) |
                                   ((0x1 << IOC) & saved_td->control_x_status) |
                                   (0x3 << C_ERR) | (0x1 << ACTIVE);
      saved_td = (TD *)m->getVirtualAddressTD(m, saved_td->pyhsicalLinkPointer &
                                                     QH_ADDRESS_MASK);
    }
    process_qh->pyhsicalQHEP =
        (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, head));
    retransmission_occured = 1;
  }

  return retransmission_occured;
}

static void control_transfer(_UHCI *uhci, UsbDev *dev, UsbDeviceRequest *rq,
                      void *data, uint8_t priority, Endpoint *endpoint,
                      build_control_transfer build_function,
                      callback_function callback, uint8_t flags) {
  uint32_t status;
  __UHC_MEMORY__(uhci, m);

  QH *qh = uhci->get_free_qh(uhci);

  qh->flags = 0;
  qh->parent = 0;
  qh->pyhsicalQHEP = 0;
  qh->pyhsicalQHLP = 0;

  uint32_t qh_physical = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, qh));
  m->addVirtualAddress(m, qh_physical, qh);

  UsbTransfer *transfer = build_function(uhci, dev, rq, data, endpoint, 
    flags & CONTROL_INITIAL_STATE);

  // we already linked them up while creating the packets
  TD *internalTD = transfer->entry_transaction->entry_packet->internalTD;
  qh->flags = (transfer->transaction_count << QH_FLAG_DEVICE_COUNT_SHIFT) |
              QH_FLAG_TYPE_CONTROL;
  qh->pyhsicalQHEP =
      ((uint32_t)(uintptr_t)(m->getPhysicalAddress(m, internalTD)));

#if defined(DEBUG_ON) || defined(TRANSFER_DEBUG_ON)
  uhci->print_USB_Transfer(uhci, transfer);
  uhci->inspect_transfer(uhci, qh, td);
#endif

  uhci->destroy_transfer(uhci, transfer);

  if ((dev->state == CONFIGURED_STATE) && !(flags & CONTROL_INITIAL_STATE)) {
    uhci->qh_to_td_map->put_c(uhci->qh_to_td_map, qh, internalTD);
    uhci->qh_data_map->put_c(uhci->qh_data_map, qh, data);
    uhci->qh_dev_map->put_c(uhci->qh_dev_map, qh, dev);
    uhci->callback_map->put_c(uhci->callback_map, qh, callback);
    uhci->qh_device_request_map->put_c(uhci->qh_device_request_map, qh, rq);
  }

  uhci->insert_queue(uhci, qh, priority, QH_CTL);

  // uhci->inspect_transfer(uhci, qh, internalTD);

  if (dev->state != CONFIGURED_STATE || ((flags & CONTROL_INITIAL_STATE) > 0)) {
    status = uhci->wait_poll(uhci, qh, UPPER_BOUND_TIME_OUT_MILLIS_CONTROL,
                             flags & SUPRESS_DEVICE_ERRORS);

    callback(dev, status, data);

    uhci->remove_queue(uhci, qh);
    m->remove_virtualAddress(m, qh_physical);
    uhci->free_qh(uhci, qh);
    uhci->remove_td_linkage(uhci, internalTD);
    dev->free_device_request(dev, rq);
  }
}

static void remove_td_linkage(_UHCI *uhci, TD *start) {
  __UHC_MEMORY__(uhci, m);
  TD *temp;

  while (start != 0) {
    uint32_t p_addr = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, start));
    temp = (TD *)m->getVirtualAddressTD(m, start->pyhsicalLinkPointer &
                                               QH_ADDRESS_MASK);
    uhci->free_td(uhci, start);
    m->remove_virtualAddressTD(m, p_addr);
    start = temp;
  }
}

static void _poll_uhci_(UsbController *controller) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);
  __UHC_MEMORY__(uhci, m);
  QH *entry = uhci->qh_entry;
  for (;;) {
    if (entry == (void *)0) {
      entry = uhci->qh_entry;
    }
    uhci->traverse_skeleton(uhci, entry);
    entry =
        (QH *)m->getVirtualAddress(m, entry->pyhsicalQHLP & QH_ADDRESS_MASK);
  }
}

static void traverse_skeleton(_UHCI *uhci, QH *entry) {
  // uhci->inspect_QH(uhci, entry);
  if (entry == (void *)0)
    return;
  if ((entry->flags & QH_FLAG_IS_MASK) == QH_FLAG_IS_MQH) {
    return;
  }

  unsigned int retransmission_occured;
  uint32_t error_mask;

  __UHC_MEMORY__(uhci, mem_service);

  // just looking for 0 would be the same result
  TD *td = (TD *)mem_service->getVirtualAddressTD(
      mem_service, entry->pyhsicalQHEP & QH_ADDRESS_MASK);

  callback_function callback =
      (callback_function)uhci->callback_map->get_c(uhci->callback_map, entry);
  void *data = (void *)uhci->qh_data_map->get_c(uhci->qh_data_map, entry);
  UsbDev *dev = (UsbDev *)uhci->qh_dev_map->get_c(uhci->qh_dev_map, entry);

  // uhci->inspect_TD(uhci, td);

  if (td == (void *)0) { // if null -> transmission was successful
    // uhci->inspect_QH(uhci, entry);
    // uhci->inspect_TD(uhci, td);

#if defined(TRANSFER_MEASURE_ON)
    if ((entry->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_BULK ||
        (entry->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_CONTROL) {
      uint32_t *i_time =
          (uint32_t *)uhci->qh_measurement->get_c(uhci->qh_measurement, entry);
      uint32_t transfer_duration;
      char *measure_msg;
#if defined(MEASURE_MS)
      measure_msg = "Control Transfer Duration in ms: %u";
      transfer_duration = getSystemTimeInMilli() - *i_time;
#elif defined(MEASURE_NS)
      measure_msg = "Control Transfer Duration in ns: %u";
      transfer_duration = getSystemTimeInNano() - *i_time;
#elif defined(MEASURE_MCS)
      measure_msg = "Control Transfer Duration in micro: %u";
      transfer_duration = getSystemTimeInMicro() - *i_time;
#endif
      mem_service->freeKernelMemory_c(mem_service, i_time, 0);
      uhci->controller_logger->info_c(uhci->controller_logger, measure_msg,
                                      transfer_duration);
    }
#endif

    callback(dev, S_TRANSFER, data);
    retransmission_occured = uhci->retransmission(uhci, entry);
    if (!retransmission_occured) {
      TD *rcvry = (TD *)uhci->qh_to_td_map->get_c(uhci->qh_to_td_map, entry);
      uhci->remove_queue(uhci, entry);
      uhci->remove_transfer_entry(uhci, entry);
      uint32_t phy_qh = (uint32_t)(uintptr_t)(mem_service->getPhysicalAddress(
          mem_service, entry));
      mem_service->remove_virtualAddress(mem_service, phy_qh);
      uhci->free_qh(uhci, entry);
      uhci->remove_td_linkage(uhci, rcvry);
    }
  }

  else if (((td->control_x_status >> ACTIVE) & 0x01) == 0x00) {
    error_mask = uhci->get_status(uhci, td);

    TD *rcvry = (TD *)uhci->qh_to_td_map->get_c(uhci->qh_to_td_map, entry);
    uhci->remove_queue(uhci, entry);
    uhci->remove_transfer_entry(uhci, entry);
    uint32_t phy_qh = (uint32_t)(uintptr_t)(mem_service->getPhysicalAddress(
        mem_service, entry));
    mem_service->remove_virtualAddress(mem_service, phy_qh);
    uhci->free_qh(uhci, entry);
    uhci->remove_td_linkage(uhci, rcvry);

    callback(dev, E_TRANSFER | error_mask, data);
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
  for (;;) {
    if (uhci->signal) {
      __UHC_MEMORY__(uhci, m);

      for (QH *qh = uhci->qh_entry; qh != 0;
           qh = (QH *)(m->getVirtualAddress(m, qh->pyhsicalQHLP &
                                                   QH_ADDRESS_MASK))) {
        uhci->traverse_skeleton(uhci, qh);
      }
      if (!uhci->signal_not_override)
        uhci->signal = 0;
      else
        uhci->signal_not_override = 0;
    } else
      yield_c();
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