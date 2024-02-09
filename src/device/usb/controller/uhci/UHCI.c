#include "UHCI.h"
#include "data/UHCI_Data.h"
#include "../../include/UsbControllerInclude.h"
#include "../UsbController.h"
#include "../UsbControllerFlags.h"
#include "components/UHCIMemory.h"
#include "components/UHCIRegister.h"
#include "../../dev/UsbDevice.h"
#include "../../dev/data/UsbDev_Data.h"
#include "../../include/UsbErrors.h"
#include "../../driver/UsbDriver.h"
#include "../../events/EventDispatcher.h"
#include "../../interfaces/IoPortInterface.h"
#include "../../interfaces/LoggerInterface.h"
#include "../../interfaces/MutexInterface.h"
#include "../../interfaces/PciDeviceInterface.h"
#include "../../interfaces/PciInterface.h"
#include "../../interfaces/SystemInterface.h"
#include "../../interfaces/ThreadInterface.h"
#include "../../interfaces/TimeInterface.h"
#include "../../utility/Utils.h"
#include "../../driver/hub/HubDriver.h"

const uint8_t CLASS_ID = 0x0C;
const uint8_t SUBCLASS_ID = 0x03;
const uint8_t INTERFACE_ID = 0x00;

// first two root ports are just hid ; later we can replace it with hubs and
// place hid to the hubs
void new_UHCI(_UHCI *uhci, PciDevice_Struct* pci_device, SystemService_C *mem_service) {
  uhci->mem_service = mem_service;
  uhci->pci_device = pci_device;

  MemoryService_C *m = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);

  uhci->init_controller_functions = &init_controller_functions;
  uhci->init_controller_functions(uhci);

  uhci->controller_logger = uhci->init_logger(uhci, m);

  uhci->dump_uhci_entry(uhci);

  uhci->addr_region = uhci->i_o_space_layout_run(uhci);

  uhci->qh_entry = uhci->request_frames(uhci);
  uhci->i_o_registers = uhci->request_register(uhci);  

  uhci->request_interrupt_service = &request_interrupt_service;    
  SystemService_C *interrupt_service =
      uhci->request_interrupt_service(uhci, mem_service);
  uhci->interrupt_service = interrupt_service; 
  
  #ifndef UHCI_POLL
  InterruptService_C* i_serv = container_of(uhci->interrupt_service, InterruptService_C, super);
  i_serv->add_interrupt_routine(i_serv, uhci->irq, &uhci->super);
  #endif

  uhci->mutex = uhci->init_mutex(uhci, m);
  uhci->init_maps(uhci, m);
  uhci->fill_maps(uhci);

  uhci->map_io_buffer_qh = (uint8_t *)m->mapIO(m, PAGE_SIZE, 1);
  uhci->map_io_buffer_td = (uint8_t *)m->mapIO(m, 2 * PAGE_SIZE, 1);

  mem_set(uhci->map_io_buffer_qh, PAGE_SIZE, 0);
  mem_set(uhci->map_io_buffer_td, 2 * PAGE_SIZE, 0);
  mem_set(uhci->map_io_buffer_bit_map_qh, PAGE_SIZE / sizeof(QH), 0);
  mem_set(uhci->map_io_buffer_bit_map_td, (2 * PAGE_SIZE) / sizeof(TD), 0);

  uhci->signal = 0;
  uhci->signal_not_override = 0;

  uhci->controller_configuration(uhci);
  create_thread("usb", &uhci->super);
}

void controller_port_configuration(_UHCI* uhci){
  MemoryService_C *m = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);

  uint16_t status;
  if ((status = ((UsbController*)uhci)->reset_port((UsbController*)uhci, 1)) > 0) {
    uhci->controller_logger->info_c(uhci->controller_logger, "Port %d enabled ...", 1);
    #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
    Register* r = uhci->look_for(uhci, Port1_Status);
    r->dump(r, uhci->controller_logger);
    #endif

    uhci->create_dev(uhci, status, 1, m);
  }

  if ((status = ((UsbController*)uhci)->reset_port((UsbController*)uhci, 2)) > 0) {
    uhci->controller_logger->info_c(uhci->controller_logger, "Port %d enabled ...", 2);
    #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
    Register* r = uhci->look_for(uhci, Port2_Status);
    r->dump(r, uhci->controller_logger);
    #endif

    uhci->create_dev(uhci, status, 2, m);
  }
}

void controller_configuration(_UHCI* uhci){
  Register *reg;
  uint8_t one_byte_command;
  uint16_t two_byte_command;

  uhci->controller_logger->info_c(uhci->controller_logger,
                                  "Performing Global Reset ...");

  reg = uhci->look_for(uhci, Usb_Command);
  // wait 5* 10 = 50 <- specs 
  for (int i = 0; i < 5; i++) {
    two_byte_command = GRESET;
    reg->write(reg, &two_byte_command);
    mdelay(USB_TDRST);
    #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
    reg->dump(reg, uhci->controller_logger);
    #endif 
    two_byte_command = 0;
    reg->write(reg, &two_byte_command);
    #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
    reg->dump(reg, uhci->controller_logger);
    #endif 
  }

  mdelay(USB_TRSTRCY);

  reg->read(reg, &two_byte_command);
  if (two_byte_command != 0x0000)
    goto fail_label;

  reg = uhci->look_for(uhci, Usb_Status);
  reg->read(reg, &two_byte_command);
  
  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 
  // HC Halted 
  if (two_byte_command != HALTED) 
    goto fail_label;

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  // write_clear
  two_byte_command = 0x00FF;
  reg->write(reg, &two_byte_command);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 


  reg = uhci->look_for(uhci, Start_of_Frame);
  reg->read(reg, &one_byte_command);
  if (one_byte_command != SOF)
    goto fail_label;

  uhci->controller_logger->info_c(uhci->controller_logger, "Performing Hardware Reset ...");

  reg = uhci->look_for(uhci, Usb_Command);
  two_byte_command = HCRESET;
  reg->write(reg, &two_byte_command);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  mdelay(10);

  reg->read(reg, &two_byte_command);
  if (two_byte_command & HCRESET)
    goto fail_label;

  uhci->controller_logger->info_c(uhci->controller_logger, "Configuring UHCI ...");

  reg = uhci->look_for(uhci, Frame_List_Base_Address);
  reg->write(reg, &uhci->fba);

  reg = uhci->look_for(uhci, Frame_Number);
  two_byte_command = 0x0000;
  reg->write(reg, &two_byte_command);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  reg = uhci->look_for(uhci, Start_of_Frame);
  one_byte_command = SOF;
  reg->write(reg, &one_byte_command);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  reg = uhci->look_for(uhci, Usb_Interrupt);
  #ifdef UHCI_POLL
  two_byte_command = 0x0000;
  uhci->controller_logger->info_c(uhci->controller_logger, "Interrupts disabled ...");
  #else
  two_byte_command = TMOUT_CRC | COMPLETE;
  uhci->controller_logger->info_c(uhci->controller_logger, "Interrupts enabled ...");
  #endif
  reg->write(reg, &two_byte_command);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  reg = uhci->look_for(uhci, Usb_Status);
  two_byte_command = 0xFFFF;
  reg->write(reg, &two_byte_command);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif

  uhci->controller_logger->info_c(uhci->controller_logger, "Configured UHCI ..."); 

  uhci->controller_logger->info_c(uhci->controller_logger, "UHCI is running ...");
  reg = uhci->look_for(uhci, Usb_Command);
  two_byte_command = MAXP | RS | CF;
  reg->write(reg, &two_byte_command);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  uhci->controller_port_configuration(uhci);

  return;

fail_label:
  uhci->controller_logger->error_c(uhci->controller_logger,
                                   "Failed to configure UHCI ...");                                  
}

void create_dev(_UHCI *uhci, int16_t status, int pn, MemoryService_C *m) {
  UsbDev *dev = (UsbDev *)m->allocateKernelMemory_c(m, sizeof(UsbDev), 0);
  uint8_t speed = (status & LOW_SPEED_ATTACH) ? LOW_SPEED : FULL_SPEED;

  dev->new_usb_device = &new_usb_device;

  uhci->controller_logger->info_c(
      uhci->controller_logger,
      "%s-Usb-Device detected at port : %d -> Start configuration",
      speed == FULL_SPEED ? "Full-Speed" : "Low-Speed", pn);

  dev->new_usb_device(dev, speed, pn, 0, 0xFF, pn, 1, (SystemService_C *)m, uhci);

  #if defined(DEVICE_DEBUG_ON) || defined(DEBUG_ON)
  dev->dump_device(dev);
  #endif

  if (dev->error_while_transfering) {
    uhci->controller_logger->error_c(
        uhci->controller_logger,
        "Aborting configuration of Usb-Device on port : %d", pn);
    return;
  }

  uhci->controller_logger->info_c(
      uhci->controller_logger, "Succesful configured Usb-Device on port : %d\n" \
      "Device : %s, %s", pn, dev->manufacturer, dev->product);

  if(dev->device_desc.bDeviceClass == HUB){
    uhci->controller_logger->info_c(uhci->controller_logger,
      "Starting enumeration on hub at port : %d: "
    );
    UsbDevice_ID usb_dev[] = {
      USB_DEVICE_INFO(HUB, 0xFF, 0xFF), {}
    };
    HubDriver* hub_driver = (HubDriver*)m->allocateKernelMemory_c(m, sizeof(HubDriver), 0);
    hub_driver->new_hub_driver = &new_hub_driver;
    hub_driver->new_hub_driver(hub_driver, "hub", usb_dev);
    uhci->super.register_driver(&uhci->super, (UsbDriver*)hub_driver);

    hub_driver->configure_hub(hub_driver);
  }    
}

void dump_uhci_entry(_UHCI* uhci){
  Logger_C* controller_logger = uhci->controller_logger;
  PciDevice_Struct* pci_device = uhci->pci_device;

  char* msg = "UHCI found : Bus = %d, device = %d, function = %d, vendorID = 0x%x, deviceID = 0x%x, IRQ = %d";

  controller_logger->info_c(controller_logger, msg, pci_device->get_bus_c(pci_device),
                    pci_device->get_device_c(pci_device), pci_device->get_function_c(pci_device),
                    pci_device->get_vendor_id_c(pci_device), pci_device->get_device_id_c(pci_device), pci_device->get_interrupt_line_c(pci_device));
}

// todo implement these methods of super !
void init_controller_functions(_UHCI *uhci) {
  uhci->super.poll = &_poll_uhci_;
  uhci->super.register_driver = &register_driver_uhci;
  uhci->super.remove_driver = &deregister_driver_uhci;
  uhci->super.control = &init_control_transfer;
  uhci->super.interrupt = &init_interrupt_transfer;
  uhci->super.bulk = &init_bulk_transfer;
  uhci->super.contains_interface = &uhci_contain_interface;
  uhci->super.is_of_type = &is_of_type_uhci;
  uhci->super.reset_port = &uhci_reset_port;
  uhci->super.link_device_to_driver = &link_device_to_driver_uhci;
  uhci->super.link_driver_to_controller = &link_driver_to_controller_uhci;
  uhci->super.link_driver_to_interface = &link_driver_to_interface_uhci;
  uhci->super.interrupt_entry_point = &interrupt_entry_point_uhci;
  uhci->super.control_entry_point = &control_entry_point_uhci;
  uhci->super.bulk_entry_point = &bulk_entry_point_uhci;
  uhci->super.new_usb_controller = &new_super_usb_controller;
  uhci->super.handler_function = &handler_function_uhci;
  uhci->super.runnable_function = &runnable_function_uhci;
  uhci->super.new_usb_controller(&uhci->super, uhci->mem_service, UHCI_name);

  uhci->dump_uhci_entry = &dump_uhci_entry;
  uhci->i_o_space_layout_run = &i_o_space_layout_run;
  uhci->request_register = &request_register;
  uhci->request_frames = &request_frames;
  uhci->look_for = &look_for_reg;
  uhci->insert_queue = &insert_queue;
  uhci->retransmission = &retransmission;
  uhci->get_status = &get_status;
  uhci->remove_queue = &remove_queue;
  uhci->wait_poll = &wait_poll;
  uhci->traverse_skeleton = &traverse_skeleton;
  uhci->create_USB_Packet = &create_USB_Packet;
  uhci->dump_all = &dump_all;
  uhci->dump_skeleton = &dump_skeleton;
  uhci->inspect_TD = &inspect_TD;
  uhci->inspect_QH = &inspect_QH;
  uhci->inspect_transfer = &inspect_transfer;
  uhci->print_USB_Transaction = &print_USB_Transaction;
  uhci->print_USB_Transfer = &print_USB_Transfer;
  uhci->control_transfer = &control_transfer;
  uhci->interrupt_transfer = &interrupt_transfer;
  uhci->bulk_transfer = &bulk_transfer;
  uhci->is_valid_priority = &is_valid_priority;
  uhci->init_control_transfer = &init_control_transfer;
  uhci->init_interrupt_transfer = &init_interrupt_transfer;
  uhci->init_bulk_transfer = &init_bulk_transfer;
  uhci->init_controller_functions = &init_controller_functions;
  uhci->init_logger = &init_logger;
  uhci->init_mutex = &init_mutex;
  uhci->init_maps = &init_maps;
  uhci->fill_maps = &fill_maps;
  uhci->create_dev = &create_dev;
  uhci->remove_transfer_entry = &remove_transfer_entry;
  uhci->get_free_qh = &get_free_qh;
  uhci->get_free_td = &get_free_td;
  uhci->free_qh = &free_qh;
  uhci->free_td = &free_td;
  uhci->remove_td_linkage = &remove_td_linkage;
  uhci->destroy_transfer = &destroy_transfer;
  uhci->controller_configuration = &controller_configuration;
  uhci->controller_port_configuration = &controller_port_configuration;
}

UsbControllerType is_of_type_uhci(UsbController *controller) {
  return TYPE_UHCI;
}

Logger_C* init_logger(_UHCI *uhci, MemoryService_C *m) {
  Logger_C *logger =
      (Logger_C *)m->allocateKernelMemory_c(m, sizeof(Logger_C), 0);
  logger->new_logger = &new_logger;

  #if defined(DEBUG_ON) || defined(TD_DEBUG_ON) || defined(QH_DEBUG_ON) \
      || defined(TRANSFER_DEBUG_ON) || defined(DEVICE_DEBUG_ON) \
      || defined(SKELETON_DEBUG_ON) || defined(REGISTER_DEBUG_ON) \
      || defined(STATUS_DEBUG_ON)

  logger->new_logger(logger, USB_CONTROLLER_LOGGER_TYPE, LOGGER_LEVEL_DEBUG);
  #else

  logger->new_logger(logger, USB_CONTROLLER_LOGGER_TYPE, LOGGER_LEVEL_INFO);

  #endif

  return logger;
}

Mutex_C* init_mutex(_UHCI* uhci, MemoryService_C* m){
  Mutex_C* mutex = (Mutex_C *)m->allocateKernelMemory_c(m, sizeof(Mutex_C), 0);
  mutex->new_mutex = &new_mutex;
  mutex->new_mutex(mutex);
  
  return mutex;
}

SystemService_C *request_interrupt_service(_UHCI* uhci, SystemService_C* mem_service) {
  InterruptService_C *interrupt_service;
  MemoryService_C *m = (MemoryService_C *)container_of(mem_service, MemoryService_C, super);

  interrupt_service = (InterruptService_C *)m->allocateKernelMemory_c(
      m, sizeof(InterruptService_C), 0);

  interrupt_service->new_interrupt_service = &new_interrupt_service;
  interrupt_service->new_interrupt_service(interrupt_service);

  return (SystemService_C *)interrupt_service;
}

void init_maps(_UHCI *uhci, MemoryService_C *m) {
  QH_TD_Map *qh_td_map = m->allocateKernelMemory_c(m, sizeof(QH_TD_Map), 0);
  QH_Callback_Function_Map *qh_callback_map =
      m->allocateKernelMemory_c(m, sizeof(QH_Callback_Function_Map), 0);
  QH_Data_Map *qh_data_map =
      m->allocateKernelMemory_c(m, sizeof(QH_Data_Map), 0);
  QH_Device_Map *qh_device_map =
      m->allocateKernelMemory_c(m, sizeof(QH_Device_Map), 0);
  Register_Map *register_map =
      m->allocateKernelMemory_c(m, sizeof(Register_Map), 0);
  QH_Device_Request_Map *device_request_map =
      m->allocateKernelMemory_c(m, sizeof(QH_Device_Request_Map), 0);

  qh_td_map->new_map = &newQH_TD;
  qh_callback_map->new_map = &newQH_CallbackMap;
  qh_data_map->new_map = &newQH_Data_Map;
  qh_device_map->new_map = &newQH_Device_Map;
  register_map->new_map = &newRegisterMap;
  device_request_map->new_map = &newQH_DeviceRequest_Map;

  qh_td_map->new_map(qh_td_map, "Map<QH*,TD*>");
  qh_callback_map->new_map(qh_callback_map, "Map<QH*,callback_function>");
  qh_data_map->new_map(qh_data_map, "Map<QH*,void*>");
  qh_device_map->new_map(qh_device_map, "Map<QH*,UsbDev*>");
  register_map->new_map(register_map, "Map<RegisterType*,Register*>");
  device_request_map->new_map(device_request_map, "Map<QH*,UsbDeviceRequest*>");

  uhci->register_look_up = (SuperMap *)register_map;
  uhci->qh_to_td_map = (SuperMap *)qh_td_map;
  uhci->callback_map = (SuperMap *)qh_callback_map;
  uhci->qh_data_map = (SuperMap *)qh_data_map;
  uhci->qh_dev_map = (SuperMap *)qh_device_map;
  uhci->qh_device_request_map = (SuperMap *)device_request_map;

  #if defined(TRANSFER_MEASURE_ON)
  QH_Measurement_Map* qh_measurement = m->allocateKernelMemory_c(m, sizeof(QH_Measurement_Map), 0);
  qh_measurement->new_map = &newQH_Measuremnt_Map;
  qh_measurement->new_map(qh_measurement, "Map<QH*,uint32_t*>");
  uhci->qh_measurement = (SuperMap*)qh_measurement;
  #endif
}

void fill_maps(_UHCI *uhci) {
  // register_map
  Register **registers = uhci->i_o_registers;
  for (int i = 0; i < 8; i++) {
    Register_Type lvalue = registers[i]->type_of(registers[i]);
    uhci->register_look_up->put_c(uhci->register_look_up, &lvalue,
                                  registers[i]);
  }
}

uint16_t uhci_reset_port(UsbController *controller, uint8_t port) {
  uint16_t val;
  uint16_t lval;
  Register *reg;

  _UHCI* uhci = (_UHCI*)container_of(controller, _UHCI, super);

  Register_Type r = (port == 1 ? Port1_Status : Port2_Status);

  reg = uhci->look_for(uhci, r);

  uhci->controller_logger->info_c(uhci->controller_logger, "Performing Reset on port = %u", port);

  reg->read(reg, &val);
  lval = val | RESET;
  reg->write(reg, &lval);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  mdelay(USB_TDRSTR);

  reg->read(reg, &val);
  lval = val & 0xFCB1;
  reg->write(reg, &lval);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  udelay(300);

  reg->read(reg, &val);
  lval = val | CON_CHANGE | CONNECT;
  reg->write(reg, &lval);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  lval = val | CONNECT | ENA;
  reg->write(reg, &lval);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  udelay(50);

  reg->read(reg, &val);
  lval = val | 0x000F;
  reg->write(reg, &lval);

  #if defined(DEBUG_ON) || defined(REGISTER_DEBUG_ON)
  reg->dump(reg, uhci->controller_logger);
  #endif 

  mdelay(50);

  reg->read(reg, &val);

  return (val & ENA);
}

QH* request_frames(_UHCI* uhci){
    QH* current;
    QH* child;

    MemoryService_C* m = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);

    uint8_t* map_io_buffer = (uint8_t*)m->mapIO(m, PAGE_SIZE, 1);
    int map_io_offset = 0;

    QH** physical_addresses = (QH**)(m->allocateKernelMemory_c(m, SKELETON_SIZE * sizeof(uint32_t*), 0));
    uint32_t* frame_list_address = (uint32_t*)(m->mapIO(m, sizeof(uint32_t) * TOTAL_FRAMES, 1));
    
    // build bulk qh
    QH* bulk_qh = (QH*)(map_io_buffer + map_io_offset);
    map_io_offset += sizeof(QH);

    bulk_qh->flags = PRIORITY_QH_8 | QH_FLAG_END | QH_FLAG_IS_MQH | QH_FLAG_TYPE_BULK;
    bulk_qh->pyhsicalQHLP = QH_TERMINATE | QH_SELECT;
    bulk_qh->pyhsicalQHEP = QH_TERMINATE | TD_SELECT;
    bulk_qh->parent = 0;

    uint32_t bulk_physical_address = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, bulk_qh));
    m->addVirtualAddress(m, bulk_physical_address, bulk_qh);

    // build control qh
    QH* control_qh = (QH*)(map_io_buffer + map_io_offset);
    map_io_offset += sizeof(QH);
    
    bulk_qh->parent = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, control_qh)); 
    
    control_qh->pyhsicalQHLP = ((uint32_t)(uintptr_t)(m->getPhysicalAddress(m, bulk_qh))) | QH_SELECT;
    control_qh->pyhsicalQHEP = QH_TERMINATE | TD_SELECT;
    control_qh->parent = 0;
    control_qh->flags = PRIORITY_QH_8 | QH_FLAG_END | QH_FLAG_IS_MQH | QH_FLAG_TYPE_CONTROL;

    uint32_t control_physical_address = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, control_qh));
    m->addVirtualAddress(m, control_physical_address, control_qh);

    #if defined(DEBUG_ON) || defined(SKELETON_DEBUG_ON)
    uhci->controller_logger->debug_c(uhci->controller_logger, "bulk qh address : %d", bulk_physical_address);
    uhci->controller_logger->debug_c(uhci->controller_logger, "bulk qhlp : %d", bulk_qh->pyhsicalQHLP & QH_ADDRESS_MASK);
    
    uhci->controller_logger->debug_c(uhci->controller_logger, "control qh address : %d", control_physical_address);
    uhci->controller_logger->debug_c(uhci->controller_logger, "control qhlp : %d", control_qh->pyhsicalQHLP & QH_ADDRESS_MASK);
    #endif
    uint32_t physical;
    // build interrupt qh
    for(int frame_number = 0; frame_number < TOTAL_FRAMES; frame_number++){
        for(int j = FRAME_SCHEDULE.size-1; j >= 0; j--){
            if((frame_number + 1) % FRAME_SCHEDULE.qh[j] == 0){
                if(frame_number + 1 == FRAME_SCHEDULE.qh[j]){
                    current = (QH*)(map_io_buffer + map_io_offset);
                    map_io_offset += sizeof(QH);

                    current->pyhsicalQHEP = 0;
                    current->pyhsicalQHLP = 0;
                    current->flags = 0;
                    current->parent = 0;

                    physical_addresses[j] = (QH*)(m->getPhysicalAddress(m, current));
                    physical = (uint32_t)(uintptr_t)physical_addresses[j];
                
                    m->addVirtualAddress(m, physical, current);
                
                    if(j == 0){
                        current->pyhsicalQHLP = ((uint32_t)(uintptr_t)(m->getPhysicalAddress(m, control_qh))) | QH_SELECT;
                        control_qh->parent = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, current));
                        frame_list_address[frame_number] = (uint32_t)(uintptr_t)(physical_addresses[j]);
                        frame_list_address[frame_number] |= QH_SELECT;
                    }
                    else{ 
                        current->pyhsicalQHLP = (uint32_t)(uintptr_t)(physical_addresses[j-1]);
                        current->pyhsicalQHLP |= QH_SELECT;
                        child->parent = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, current));
                        frame_list_address[frame_number] = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, current));
                        frame_list_address[frame_number] |= QH_SELECT;
                    }
                    current->pyhsicalQHEP = TD_SELECT | QH_TERMINATE;
                    current->flags = PRIORITY_QH_8 | QH_FLAG_END | QH_FLAG_IS_MQH | QH_FLAG_TYPE_INTERRUPT;

                    #if defined(DEBUG_ON) || defined(SKELETON_DEBUG_ON)
                    uhci->controller_logger->debug_c(uhci->controller, "address : %d", physical);
                    uhci->controller_logger->debug_c(uhci->controller, "qhlp : %d", current->pyhsicalQHLP & QH_ADDRESS_MASK);
                    #endif

                    child = current;
                    break;
                }
                frame_list_address[frame_number] = (uint32_t)(uintptr_t)(physical_addresses[j]);
                frame_list_address[frame_number] |= QH_SELECT;
            }
        }
    }
    m->freeKernelMemory_c(m, physical_addresses, 0);

    uhci->fba = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, frame_list_address));
    return current;
}

Addr_Region* i_o_space_layout_run(_UHCI* uhci){
    uint16_t command;
    IO_Port_Struct_C* io_port;
    IO_Region* io_region;
    uint16_t base_address;

    MemoryService_C* m = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);
    PciDevice_Struct* pci_device = uhci->pci_device;

    command = uhci->pci_device->readWord_c(uhci->pci_device, COMMAND);

    command = pci_device->readWord_c(pci_device, COMMAND);
    command &= (0xFFFF ^ (INTERRUPT_DISABLE | MEMORY_SPACE));
    command |= BUS_MASTER | IO_SPACE;
    pci_device->writeWord_c(pci_device, COMMAND, command);

    base_address = pci_device->readDoubleWord_c(pci_device, BASE_ADDRESS_4) & 0xFFFFFFFC;
    uhci->irq = pci_device->readByte_c(pci_device, INTERRUPT_LINE);

    pci_device->writeDoubleWord_c(pci_device, CAPABILITIES_POINTER, 0x00000000);
    pci_device->writeDoubleWord_c(pci_device, 0x38, 0x00000000);

    pci_device->writeWord_c(pci_device, 0xC0, 0x8F00);
    pci_device->writeWord_c(pci_device, 0xC0, 0x2000);

    io_port = (IO_Port_Struct_C*)m->allocateKernelMemory_c(m, sizeof(IO_Port_Struct_C), 0);
    io_port->newIO_Port = &newIO_Port;
    io_port->newIO_Port(io_port, base_address);

    io_region = (IO_Region*)m->allocateKernelMemory_c(m, sizeof(IO_Region),0);
    io_region->new_io_region = &new_io_region;

    io_region->new_io_region(io_region, io_port, 8);

    return (Addr_Region*)io_region;
}

Register** request_register(_UHCI* uhci){
    MemoryService_C* m = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);
    Addr_Region* ar = uhci->addr_region;

    Command_Register* c_reg = (Command_Register*)m->allocateKernelMemory_c(m, sizeof(Command_Register), 0);
    Status_Register* s_reg = (Status_Register*)m->allocateKernelMemory_c(m, sizeof(Status_Register), 0);
    Interrupt_Register* i_reg = (Interrupt_Register*)m->allocateKernelMemory_c(m, sizeof(Interrupt_Register), 0);
    Port_Register* p_reg_1 = (Port_Register*)m->allocateKernelMemory_c(m, sizeof(Port_Register), 0);
    Port_Register* p_reg_2 = (Port_Register*)m->allocateKernelMemory_c(m, sizeof(Port_Register), 0);
    Frame_Numb_Register* f_n_reg = (Frame_Numb_Register*)m->allocateKernelMemory_c(m, sizeof(Frame_Numb_Register), 0);
    Frame_Base_Register* f_b_reg = (Frame_Base_Register*)m->allocateKernelMemory_c(m, sizeof(Frame_Base_Register), 0);
    SOF_Register* sof_reg = (SOF_Register*)m->allocateKernelMemory_c(m, sizeof(SOF_Register), 0);

    Register** regs = (Register**)m->allocateKernelMemory_c(m, sizeof(Register*) * 8, 0);

    c_reg->new_command_register = &new_command_reg;
    s_reg->new_status_register = &new_status_reg;
    i_reg->new_interrupt_register = &new_interrupt_reg;
    p_reg_1->new_port_register = &new_port_reg;
    p_reg_2->new_port_register = &new_port_reg;
    f_n_reg->new_frame_number_register = &new_frame_number_reg;
    f_b_reg->new_frame_base_register = &new_frame_base_reg;
    sof_reg->new_sof_register = &new_sof_reg;

    c_reg->new_command_register(c_reg, ar, (uint16_t*)m->allocateKernelMemory_c(m, sizeof(uint16_t),0));
    s_reg->new_status_register(s_reg, ar, (uint16_t*)m->allocateKernelMemory_c(m, sizeof(uint16_t),0));
    i_reg->new_interrupt_register(i_reg, ar, (uint16_t*)m->allocateKernelMemory_c(m, sizeof(uint16_t),0));
    p_reg_1->new_port_register(p_reg_1, 1, ar, (uint16_t*)m->allocateKernelMemory_c(m, sizeof(uint16_t),0));
    p_reg_2->new_port_register(p_reg_2, 2, ar, (uint16_t*)m->allocateKernelMemory_c(m, sizeof(uint16_t),0));
    f_n_reg->new_frame_number_register(f_n_reg, ar, (uint16_t*)m->allocateKernelMemory_c(m, sizeof(uint16_t),0));
    f_b_reg->new_frame_base_register(f_b_reg, ar, (uint32_t*)m->allocateKernelMemory_c(m, sizeof(uint32_t),0));
    sof_reg->new_sof_register(sof_reg, ar, (uint8_t*)m->allocateKernelMemory_c(m, sizeof(uint8_t),0));

    regs[0] = (Register*)c_reg;
    regs[1] = (Register*)s_reg;
    regs[2] = (Register*)i_reg;
    regs[3] = (Register*)f_n_reg;
    regs[4] = (Register*)f_b_reg;
    regs[5] = (Register*)sof_reg;
    regs[6] = (Register*)p_reg_1;
    regs[7] = (Register*)p_reg_2;

    return regs;
}

/*Addr_Region *memory_space_layout_run(_UHCI_Builder* builder, PciDevice_Struct* pci_device){
    Memory_Region *addr_region;
    uint16_t command;
    uint32_t bar_4, size;
    char *v_addr;
    char *p_addr;
    uint32_t stored_bar;

    MemoryService_C* m = (MemoryService_C*)container_of(builder->mem_service, MemoryService_C, super);

    command = pci_device->readWord_c(pci_device, COMMAND);
    command |= BUS_MASTER | MEMORY_SPACE;
    pci_device->writeWord_c(pci_device, COMMAND, command);

    stored_bar = pci_device->readDoubleWord_c(pci_device, BASE_ADDRESS_4);

    addr_region = m->allocateKernelMemory_c(m, sizeof(Memory_Region), 0);
    pci_device->writeDoubleWord_c(pci_device, BASE_ADDRESS_4, 0xFFFFFFFF);
    bar_4 = pci_device->readDoubleWord_c(pci_device, BASE_ADDRESS_4) & 0xFFFFFFF0;
    size = (~(bar_4)) + 1;

    v_addr = (char *)m->mapIO(m, size, 1);

    p_addr = (char *)m->getPhysicalAddress(m, v_addr);
    bar_4 = ((uint32_t)(uintptr_t)p_addr) | (stored_bar & 0x0000000F);
    pci_device->writeDoubleWord_c(pci_device, BASE_ADDRESS_4, bar_4);    

    addr_region->new_memory_region = &new_mem_region;
    
    addr_region->new_memory_region(addr_region,v_addr, size);

    return (Addr_Region*)addr_region;
}*/

QH *get_free_qh(_UHCI *uhci) {
  for (int i = 0; i < PAGE_SIZE / sizeof(QH); i++) {
    //uhci->mutex->acquire_c(uhci->mutex);
    if (uhci->map_io_buffer_bit_map_qh[i] == 0) {
      uhci->map_io_buffer_bit_map_qh[i] = 1;
      //uhci->mutex->release_c(uhci->mutex);
      return (QH *)(uhci->map_io_buffer_qh + (i * sizeof(QH)));
    }
    //uhci->mutex->release_c(uhci->mutex);
  }
  return (void *)0;
}

TD *get_free_td(_UHCI *uhci) {
  for (int i = 0; i < ((2 * PAGE_SIZE) / sizeof(TD)); i++) {
    //uhci->mutex->acquire_c(uhci->mutex);
    if (uhci->map_io_buffer_bit_map_td[i] == 0) {
      uhci->map_io_buffer_bit_map_td[i] = 1;
      //uhci->mutex->release_c(uhci->mutex);
      return (TD *)(uhci->map_io_buffer_td + (i * sizeof(TD)));
    }
    //uhci->mutex->release_c(uhci->mutex);
  }
  return (void *)0;
}

void free_qh(_UHCI *uhci, QH *qh) {
  for (int i = 0; i < PAGE_SIZE; i += sizeof(QH)) {
    //uhci->mutex->acquire_c(uhci->mutex);
    if ((uhci->map_io_buffer_qh + i) == (uint8_t *)qh) {
      uhci->map_io_buffer_bit_map_qh[i / sizeof(QH)] = 0;
      //uhci->mutex->release_c(uhci->mutex);
      return;
    }
    //uhci->mutex->release_c(uhci->mutex);
  }
}

void free_td(_UHCI *uhci, TD *td) {
  for (int i = 0; i < (2 * PAGE_SIZE); i += sizeof(TD)) {
    //uhci->mutex->acquire_c(uhci->mutex);
    if ((uhci->map_io_buffer_td + i) == (uint8_t *)td) {
      uhci->map_io_buffer_bit_map_td[i / sizeof(TD)] = 0;
      //uhci->mutex->release_c(uhci->mutex);
      return;
    }
    //uhci->mutex->release_c(uhci->mutex);
  }
}

void insert_queue(_UHCI *uhci, QH *new_qh, TD *td,
                  uint16_t priority, enum QH_HEADS v) {
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
  uint32_t physical_addr;
  int8_t offset = (int8_t)v;
  QH *current = uhci->qh_entry;
  
  uhci->mutex->acquire_c(uhci->mutex);
  
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
  if((new_qh->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_BULK ||
     (new_qh->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_CONTROL){
    uint32_t* measure = (uint32_t*)m->allocateKernelMemory_c(m, sizeof(uint32_t), 0);
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
  uhci->mutex->release_c(uhci->mutex);
}

// todo as in the insert queue method
void remove_queue(_UHCI *uhci, QH *qh) {
  MemoryService_C *memory_service = (MemoryService_C *)container_of(
      uhci->mem_service, MemoryService_C, super);

  uhci->mutex->acquire_c(uhci->mutex);

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

  uhci->mutex->release_c(uhci->mutex);
}

Register *look_for_reg(_UHCI *uhci, Register_Type r) {
  return (
      Register *)(uhci->register_look_up->get_c(uhci->register_look_up, &r));
}

/*Interface* lock_interface(_UHCI* uhci, UsbDev* dev, unsigned int
interface_num){ return dev->usb_dev_interface_lock(dev, interface_num);
}*/

void dump_drivers(_UHCI *uhci) {
  list_element *l_e = uhci->super.head_driver.l_e;
  Logger_C *logger = uhci->controller_logger;
  char *message = "UHCI : %x\n";
  char *driver_message = "\tDriver : %s\n";
  char *device_message = "\t\tDevice : %s\n";

  logger->debug_c(logger, message, (uint32_t)(uintptr_t)uhci);
  while (l_e != (void *)0) {
    UsbDriver *usb_driver = (UsbDriver *)container_of(l_e, UsbDriver, l_e);
    logger->debug_c(logger, driver_message, usb_driver->name);
    list_element *l = usb_driver->head.l_e;
    while (l != (void *)0) {
      UsbDev *dev = (UsbDev *)container_of(l, UsbDev, l_e_driver);
      logger->debug_c(logger, device_message, dev->device_desc.idProduct);
      l = l->l_e;
    }
    l_e = l_e->l_e;
  }
}

void dump_devices(_UHCI *uhci) {
  list_element *l_e = uhci->super.head_dev.l_e;
  Logger_C *logger = uhci->controller_logger;
  char *message = "UHCI : %x\n";
  char *device_message = "\tDevice : %s\n";
  char *interface_message = "\t\t%d.%d : %s";

  logger->debug_c(logger, message, (uint32_t)(uintptr_t)uhci);
  while (l_e != (void *)0) {
    UsbDev *usb_dev = (UsbDev *)container_of(l_e, UsbDev, l_e);
    logger->debug_c(logger, device_message, usb_dev->device_desc.idProduct);
    int config_value = usb_dev->active_config->config_desc.bConfigurationValue;
    int interface_num = usb_dev->active_config->config_desc.bNumInterfaces;
    Interface **interfaces = usb_dev->active_config->interfaces;
    for (int i = 0; i < interface_num; i++) {
      int interface_number =
          interfaces[i]
              ->active_interface->alternate_interface_desc.bInterfaceNumber;
      UsbDriver *used_driver = (UsbDriver *)interfaces[i]->driver;
      logger->debug_c(logger, interface_message, config_value, interface_number,
                      used_driver->name);
    }
    l_e = l_e->l_e;
  }
}

void free_interface(_UHCI *uhci, UsbDev *dev, Interface *interface) {
  dev->usb_dev_free_interface(dev, interface);
}

int uhci_contain_interface(UsbController *controller, Interface *interface) {

  return controller->interface_dev_map->get_c(controller->interface_dev_map, interface) ==
                 (void *)0
             ? -1
             : 1;
}

// replace each method with prototyp (..., UsbDev*, Interface*, ...) -> (...,
// Interface*, ...);
// entry point for usb driver
void init_control_transfer(UsbController *controller, Interface *interface,
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

  dev->usb_dev_control(dev, interface, pipe, priority, data, setup, callback, 0);
}

void init_interrupt_transfer(UsbController *controller, Interface *interface,
                             unsigned int pipe, uint8_t priority, void *data,
                             unsigned int len, uint16_t interval,
                             callback_function callback) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);

  UsbDev *dev =
      controller->interface_dev_map->get_c(controller->interface_dev_map, interface);

  if (dev == (void *)0)
    callback(0, E_INTERFACE_INV, data);

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

  enum QH_HEADS mqh;

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
    if (interval <= 0) {
      callback(dev, E_INVALID_INTERVAL, data);
      return;
    }
    interval = floor_address(interval);

    uhci->init_interrupt_transfer((UsbController *)uhci, interface, pipe,
                                  priority, data, len, interval, callback);
    return;
  }

  dev->usb_dev_interrupt(dev, interface, pipe, (uint16_t)shifted_prio, data,
                         len, (uint8_t)mqh, callback);
}

int16_t is_valid_priority(_UHCI *uhci, UsbDev *dev, uint8_t priority,
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

void init_bulk_transfer(UsbController *controller, Interface *interface,
                        unsigned int pipe, uint8_t priority, void *data,
                        unsigned len, callback_function callback) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);

  UsbDev *dev =
      controller->interface_dev_map->get_c(controller->interface_dev_map, interface);

  if (dev == (void *)0)
    callback(0, E_INTERFACE_INV, data);

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

int register_driver_uhci(UsbController *controller, UsbDriver *driver) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);
  list_element *first_dev = uhci->super.head_dev.l_e;
  UsbDevice_ID *device_id_table = driver->entry;

  if (driver->probe == (void *)0) {
    return -1;
  };
  // driver should be listed, althought i does not have a matching dev
  // useful when later having plug & play
  ((UsbController*)uhci)->link_driver_to_controller((UsbController*)uhci, driver);

  int driver_device_match_count = 0;

  for (int i = 0;
       device_id_table[i].idVendor != 0 || device_id_table[i].idProduct != 0 ||
       device_id_table[i].bcdDevice_low != 0 ||
       device_id_table[i].bcdDevice_high != 0 ||
       device_id_table[i].bDeviceClass != 0 ||
       device_id_table[i].bDeviceSubClass != 0 ||
       device_id_table[i].bDeviceProtocol != 0 ||
       device_id_table[i].bInterfaceClass != 0 ||
       device_id_table[i].bInterfaceSubClass != 0 ||
       device_id_table[i].bInterfaceProtocol != 0;
       i++) {
    UsbDevice_ID device_id = device_id_table[i];
    for (list_element *copy_devs = first_dev; copy_devs != (void *)0;
         copy_devs = copy_devs->l_e) {
      UsbDev *dev = (UsbDev *)container_of(copy_devs, UsbDev, l_e);
      DeviceDescriptor device_desc = dev->device_desc;
      // 0xFF is default value -> if change check
      if (device_id.idVendor != 0xFF &&
          device_id.idVendor != device_desc.idVendor) {
        continue;
      }
      if (device_id.idProduct != 0xFF &&
          device_id.idProduct != device_desc.idProduct) {
        continue;
      }
      if (device_id.bcdDevice_high != 0xFF &&
          device_id.bcdDevice_high !=
              USB_MAJOR_VERSION(device_desc.bcdDevice) &&
          device_id.bcdDevice_low != USB_MINOR_VERSION(device_desc.bcdDevice)) {
        continue;
      }
      if (device_id.bDeviceClass != 0xFF &&
          device_id.bDeviceClass != device_desc.bDeviceClass) {
        continue;
      }
      if (device_id.bDeviceSubClass != 0xFF &&
          device_id.bDeviceSubClass != device_desc.bDeviceSubClass) {
        continue;
      }
      if (device_id.bDeviceProtocol != 0xFF &&
          device_id.bDeviceProtocol != device_desc.bDeviceProtocol) {
        continue;
      }

      Configuration *config = dev->active_config;
      int interface_num = config->config_desc.bNumInterfaces;

      for (int k = 0; k < interface_num; k++) {
        Interface *interface = config->interfaces[k];
        Alternate_Interface *alt_interface = interface->active_interface;
        if (device_id.bInterfaceClass != 0xFF &&
            device_id.bInterfaceClass !=
                alt_interface->alternate_interface_desc.bInterfaceClass) {
          continue;
        }
        if (device_id.bInterfaceSubClass != 0xFF &&
            device_id.bInterfaceSubClass !=
                alt_interface->alternate_interface_desc.bInterfaceSubClass) {
          continue;
        }
        if (device_id.bInterfaceProtocol != 0xFF &&
            device_id.bInterfaceProtocol !=
                alt_interface->alternate_interface_desc.bInterfaceProtocol) {
          continue;
        }

        int status = dev->usb_dev_interface_lock(dev, interface, driver);

        if ((status == E_INTERFACE_IN_USE) || (status == E_INTERFACE_INV)) {
          continue;
        }

        if (driver->probe(dev, interface) < 0) {
          dev->usb_dev_free_interface(dev, interface);
        } else {
          driver_device_match_count++;
          ((UsbController*)uhci)->link_device_to_driver((UsbController*)uhci, dev, driver);
          ((UsbController*)uhci)->link_driver_to_interface((UsbController*)uhci, driver, interface);
          ((UsbController*)uhci)->interface_dev_map->put_c(
              ((UsbController*)uhci)->interface_dev_map, interface,
              dev);
          driver->dispatcher = uhci->super.dispatcher;    
        }
      }
    }
  }
  return (driver_device_match_count == 0
              ? -1
              : driver_device_match_count); // no device found if 0
}

void link_device_to_driver_uhci(UsbController *controller, UsbDev *dev, UsbDriver *driver) {
  list_element *l_e = driver->head.l_e;

  _UHCI* uhci = (_UHCI*)container_of(controller, _UHCI, super);

  //uhci->mutex->acquire_c(uhci->mutex);

  if (l_e == (void *)0) {
    driver->head.l_e = &dev->l_e_driver;
    //uhci->mutex->release_c(uhci->mutex);
    return;
  }
  while (l_e->l_e != (void *)0) {
    l_e = l_e->l_e;
  }

  //uhci->mutex->release_c(uhci->mutex);

  l_e->l_e = &dev->l_e_driver;
}

void link_driver_to_controller_uhci(UsbController* controller, UsbDriver *driver) {
  _UHCI* uhci = (_UHCI*)container_of(controller, _UHCI, super);
  list_element *l_e = uhci->super.head_driver.l_e;
  
  //uhci->mutex->acquire_c(uhci->mutex);
  
  if (l_e == (void *)0) {
    uhci->super.head_driver.l_e = &driver->l_e;
    //uhci->mutex->release_c(uhci->mutex);
    return;
  }
  while (l_e->l_e != (void *)0) {
    l_e = l_e->l_e;
  }

  //uhci->mutex->release_c(uhci->mutex);

  l_e->l_e = &driver->l_e;
}

void link_driver_to_interface_uhci(UsbController* controller, UsbDriver *driver,
                              Interface *interface) {
  interface->driver = (void *)driver;
}

int deregister_driver_uhci(UsbController *controller, UsbDriver *driver) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);

  list_element *prev;
  list_element *l_e = uhci->super.head_driver.l_e;

  UsbDriver *d = (UsbDriver *)container_of(l_e, UsbDriver, l_e);

  if (l_e == (void *)0)
    return -1;

  //uhci->mutex->acquire_c(uhci->mutex);

  if (d == driver) {
    uhci->super.head_driver.l_e = uhci->super.head_driver.l_e->l_e;
  } else {
    prev = l_e;
    l_e = l_e->l_e;
    // remove controller linkage
    while (l_e != (void *)0) {
      d = (UsbDriver *)container_of(l_e, UsbDriver, l_e);
      if (driver == d) {
        prev->l_e = l_e->l_e;
        break;
      }
      l_e = l_e->l_e;
    }
  }

  // remove device linkage
  list_element *l_dev = uhci->super.head_dev.l_e;
  while (l_dev != (void *)0) {
    UsbDev *dev = (UsbDev *)container_of(l_dev, UsbDev, l_e);
    int interface_num = dev->active_config->config_desc.bNumInterfaces;
    Interface **interfaces = dev->active_config->interfaces;
    for (int i = 0; i < interface_num; i++) {
      if (((UsbDriver *)interfaces[i]->driver) == driver) {
        dev->usb_dev_free_interface(dev, interfaces[i]);
        //uhci->mutex->release_c(uhci->mutex);
        return 1;
      }
    }
    l_dev = l_dev->l_e;
  }

  //uhci->mutex->release_c(uhci->mutex);

  return -1;
}

void bulk_entry_point_uhci(UsbDev *dev, Endpoint *endpoint, void *data,
                      unsigned int len, uint8_t priority,
                      callback_function callback, uint8_t flags) {
  _UHCI *uhci_rcvry = (_UHCI*)container_of((UsbController*)dev->controller, _UHCI, super);

  uhci_rcvry->bulk_transfer(uhci_rcvry, dev, data, len, priority, endpoint,
                            &build_interrupt_or_bulk, callback, flags);
}

void control_entry_point_uhci(UsbDev *dev, UsbDeviceRequest *device_request,
                         void *data, uint8_t priority, Endpoint *endpoint,
                         callback_function callback, uint8_t flags) {
  _UHCI *uhci_rcvry = (_UHCI *)container_of(((UsbController*)dev->controller), _UHCI, super);

  uhci_rcvry->control_transfer(uhci_rcvry, dev, device_request, data, priority,
                               endpoint, &build_control, callback, flags);
}

void interrupt_entry_point_uhci(UsbDev *dev, Endpoint *endpoint, void *data,
                           unsigned int len, uint8_t priority,
                           uint16_t interval, callback_function callback) {
  _UHCI *uhci_rcvry = container_of((UsbController*)dev->controller, _UHCI, super);

  uhci_rcvry->interrupt_transfer(uhci_rcvry, dev, data, len, interval, priority,
                                 endpoint, &build_interrupt_or_bulk, callback);
}

void use_alternate_setting(_UHCI *uhci, UsbDev *dev, Interface *interface,
                           unsigned int setting, callback_function callback) {
  dev->request_switch_alternate_setting(dev, interface, setting, callback);
}

void switch_configuration(_UHCI *uhci, UsbDev *dev, int configuration,
                          callback_function callback) {
  dev->request_switch_configuration(dev, configuration, callback);
}

UsbTransfer *build_interrupt_or_bulk(_UHCI *uhci, UsbDev *dev, void *data,
                                     Endpoint *e, unsigned int len,
                                     const char *type, uint8_t flags) {
  UsbPacket *prev = 0;

  UsbTransaction *prev_transaction = 0;
  UsbTransaction *data_transaction = 0;

  data_transaction->next = 0; 

  TokenValues token;
  int count = 0;
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
  UsbTransfer *usb_transfer =
      (UsbTransfer*)m->allocateKernelMemory_c(m, sizeof(UsbTransfer), 0);

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

void print_USB_Transfer(_UHCI *uhci, UsbTransfer *transfer) {
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
  uhci->controller_logger->debug_c(uhci->controller_logger, message,
                                   transfer->transfer_type);
}

void print_USB_Transaction(_UHCI *uhci, UsbTransaction *transaction,
                           int order) {
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
  char *message = "Transaction :\nType : %s\nOrder : %d\n"
                  "Packet : %u\n\n"; // packet contains the address of the td
  uint32_t address = (uint32_t)(uintptr_t)(m->getPhysicalAddress(
      m, transaction->entry_packet));
  uhci->controller_logger->debug_c(uhci->controller_logger, message,
                                   transaction->transaction_type, order,
                                   address);
}

void inspect_QH(_UHCI *uhci, struct QH *qh) {
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
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
  uhci->controller_logger->debug_c(
      uhci->controller_logger, message, address, qh->pyhsicalQHLP,
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

void inspect_TD(_UHCI *uhci, struct TD *td) {
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
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
  uhci->controller_logger->debug_c(
      uhci->controller_logger, message, address, td->pyhsicalLinkPointer,
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

void inspect_transfer(_UHCI *uhci, QH *qh, TD *td) {
  MemoryService_C *memory_service = (MemoryService_C *)container_of(
      uhci->mem_service, MemoryService_C, super);

  uhci->inspect_QH(uhci, qh);

  while (td != (void *)0) {
    uhci->inspect_TD(uhci, td);
    td = (TD *)memory_service->getVirtualAddressTD(
        memory_service, td->pyhsicalLinkPointer & QH_ADDRESS_MASK);
  }
}

void dump_skeleton(_UHCI *uhci) {
  QH *qh = uhci->qh_entry;
  MemoryService_C* memory_service = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);

  while (qh != (void *)0) {
    uhci->inspect_QH(uhci, qh);

    qh = (QH*)memory_service->getVirtualAddress(memory_service, qh->pyhsicalQHLP & QH_ADDRESS_MASK);
  }
}

void dump_all(_UHCI *uhci) {
  MemoryService_C* mem_service = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);
  QH *qh = uhci->qh_entry;
  TD *td;

  while (qh != (void *)0) {
    uhci->inspect_QH(uhci, qh);
    td = (TD *)mem_service->getVirtualAddressTD(mem_service, qh->pyhsicalQHEP & QH_ADDRESS_MASK);
    while (td != (void *)0) {
      uhci->inspect_TD(uhci, td);
      td = (TD *)mem_service->getVirtualAddressTD(mem_service, td->pyhsicalLinkPointer & QH_ADDRESS_MASK);
    }
    qh = (QH *)mem_service->getVirtualAddress(mem_service, qh->pyhsicalQHLP & QH_ADDRESS_MASK);
  }
}

UsbTransfer *build_control(_UHCI *uhci, UsbDev *dev,
                           UsbDeviceRequest *device_request, void *data,
                           Endpoint *e, uint8_t flags) {
  UsbPacket *head = 0;
  UsbPacket *prev = 0;

  UsbTransaction *prev_transaction = 0;
  UsbTransaction *data_transaction = 0;

  TokenValues token;
  unsigned int count = 0;
  unsigned int endpoint =
      (e == ((void *)0) ? 0
                        : e->endpoint_desc.bEndpointAddress & ENDPOINT_MASK);

  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
  UsbTransfer *usb_transfer =
      (UsbTransfer*)m->allocateKernelMemory_c(m, sizeof(UsbTransfer), 0);

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
    prev =
        uhci->create_USB_Packet(uhci, dev, prev, token, dev->speed, start, 0, 0);
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
UsbPacket *create_USB_Packet(_UHCI *uhci, UsbDev *dev, UsbPacket *prev,
                             TokenValues token, int8_t speed, void *data,
                             int last_packet, uint8_t flags) {
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);

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

    if((dev->state == CONFIGURED_STATE) && (flags != BULK_INITIAL_STATE) && 
        (flags != CONTROL_INITIAL_STATE)){
      td->control_x_status |= (0x1 << IOC); // set interrupt bit only for last td in qh !
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

// insert flag CONFIGURED : used for not using interrupts instead poll for timeout -> like the controls
void bulk_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                   uint8_t priority, Endpoint *e,
                   build_bulk_or_interrupt_transfer build_function,
                   callback_function callback, uint8_t flags) {
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);

  QH *qh = uhci->get_free_qh(uhci);

  qh->flags = 0;
  qh->parent = 0;
  qh->pyhsicalQHEP = 0;
  qh->pyhsicalQHLP = 0;

  uint32_t qh_physical = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, qh));
  m->addVirtualAddress(m, qh_physical, qh);

  UsbTransfer *transfer =
      build_function(uhci, dev, data, e, len, BULK_TRANSFER, flags);

  TD *td = transfer->entry_transaction->entry_packet->internalTD;

  qh->flags = (transfer->transaction_count << QH_FLAG_DEVICE_COUNT_SHIFT) |
              QH_FLAG_TYPE_BULK;

  qh->pyhsicalQHEP = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, td));

  if(flags != BULK_INITIAL_STATE){
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

  uhci->insert_queue(uhci, qh, td, priority, QH_BULK);

  if(flags == BULK_INITIAL_STATE){
    uint32_t status = uhci->wait_poll(uhci, qh, UPPER_BOUND_TIME_OUT_MILLIS_BULK);

    callback(dev, status, data);

    uhci->remove_queue(uhci, qh);
    m->remove_virtualAddress(m, qh_physical);
    uhci->free_qh(uhci, qh);
    uhci->remove_td_linkage(uhci, td);
  }
}

void interrupt_transfer(_UHCI *uhci, UsbDev *dev, void *data, unsigned int len,
                        uint16_t interval, uint8_t priority, Endpoint *e,
                        build_bulk_or_interrupt_transfer build_function,
                        callback_function callback) {
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);

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

  uhci->insert_queue(uhci, qh, td, priority, (enum QH_HEADS)interval);
}

// wait time out time -> upper bound 10ms 
uint32_t wait_poll(_UHCI *uhci, QH *process_qh, uint32_t timeout) {
  TD *td;

  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
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

  if(td != (void*)0){ // transfer not sucessful
    status |= uhci->get_status(uhci, td);
    uhci->controller_logger->error_c(uhci->controller_logger, "transfer was not sucessfult ! error mask %u", status);
  }

  #if defined(TRANSFER_MEASURE_ON)
  uint32_t* i_time = (uint32_t*)uhci->qh_measurement->get_c(uhci->qh_measurement, process_qh);
  uint32_t transfer_duration;
  char* measure_msg;
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
  m->freeKernelMemory_c(m, i_time, 0);
  uhci->controller_logger->info_c(uhci->controller_logger, measure_msg, transfer_duration);
  #endif
  
  return status;
}

uint32_t get_status(_UHCI *uhci, TD *td) {
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
unsigned int retransmission(_UHCI *uhci, QH *process_qh, TD *td) {
  TD *saved_td = 0;
  TD *head = 0;

  unsigned int retransmission_occured = 0;
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
  uint8_t transfer_type = process_qh->flags & QH_FLAG_TYPE_MASK;
  if ((td == (void *)0) && (transfer_type == QH_FLAG_TYPE_INTERRUPT)) {
    saved_td = (TD *)uhci->qh_to_td_map->get_c(uhci->qh_to_td_map, process_qh);
    head = saved_td;
    while (saved_td != (void *)0) {
      saved_td->control_x_status = ((0x1 << LS) & saved_td->control_x_status) |
                                   ((0x1 << IOC) & saved_td->control_x_status) | (0x3 << C_ERR) | (0x1 << ACTIVE);
      saved_td = (TD *)m->getVirtualAddressTD(m, saved_td->pyhsicalLinkPointer &
                                                     QH_ADDRESS_MASK);
    }
    process_qh->pyhsicalQHEP =
        (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, head));
    retransmission_occured = 1;
  } else if (td != (void *)0) {
    td->control_x_status = ((0x1 << LS) & td->control_x_status) | ((0x1 << IOC) & td->control_x_status) | (0x3 << C_ERR) | (0x1 << ACTIVE);
    retransmission_occured = 1;
  }
  return retransmission_occured;
}

void control_transfer(_UHCI *uhci, UsbDev *dev, UsbDeviceRequest *rq,
                      void *data, uint8_t priority, Endpoint *endpoint,
                      build_control_transfer build_function,
                      callback_function callback, uint8_t flags) {
  uint32_t status;
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);

  QH *qh = uhci->get_free_qh(uhci);

  qh->flags = 0;
  qh->parent = 0;
  qh->pyhsicalQHEP = 0;
  qh->pyhsicalQHLP = 0;

  uint32_t qh_physical = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, qh));
  m->addVirtualAddress(m, qh_physical, qh);

  UsbTransfer *transfer = build_function(uhci, dev, rq, data, endpoint, flags);

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

  if(dev->state == CONFIGURED_STATE){
    uhci->qh_to_td_map->put_c(uhci->qh_to_td_map, qh, internalTD);
    uhci->qh_data_map->put_c(uhci->qh_data_map, qh, data);
    uhci->qh_dev_map->put_c(uhci->qh_dev_map, qh, dev);
    uhci->callback_map->put_c(uhci->callback_map, qh, callback);
    uhci->qh_device_request_map->put_c(uhci->qh_device_request_map, qh, rq);
  }

  uhci->insert_queue(uhci, qh, internalTD, priority, QH_CTL);

  // uhci->inspect_transfer(uhci, qh, internalTD);

  if (dev->state != CONFIGURED_STATE || flags == CONTROL_INITIAL_STATE) {
    status = uhci->wait_poll(uhci, qh, UPPER_BOUND_TIME_OUT_MILLIS_CONTROL);
    
    callback(dev, status, data);
    
    uhci->remove_queue(uhci, qh);
    m->remove_virtualAddress(m, qh_physical);
    uhci->free_qh(uhci, qh);
    uhci->remove_td_linkage(uhci, internalTD);
    dev->free_device_request(dev, rq);
  }
}

void remove_td_linkage(_UHCI *uhci, TD *start) {
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
  TD* temp;

  while (start != 0) {
    uint32_t p_addr = (uint32_t)(uintptr_t)(m->getPhysicalAddress(m, start));
    temp = (TD *)m->getVirtualAddressTD(m, start->pyhsicalLinkPointer &
                                                QH_ADDRESS_MASK);
    uhci->free_td(uhci, start);
    m->remove_virtualAddressTD(m, p_addr);
    start = temp;
  }
}

void _poll_uhci_(UsbController *controller) {
  _UHCI *uhci = (_UHCI *)container_of(controller, _UHCI, super);
  MemoryService_C *m = (MemoryService_C *)container_of(uhci->mem_service,
                                                       MemoryService_C, super);
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

void traverse_skeleton(_UHCI *uhci, QH *entry) {
  // uhci->inspect_QH(uhci, entry);
  if(entry == (void*)0) return;
  if ((entry->flags & QH_FLAG_IS_MASK) == QH_FLAG_IS_MQH) {
    return;
  }

  unsigned int retransmission_occured;
  uint32_t error_mask;

  MemoryService_C *mem_service = (MemoryService_C *)container_of(
      uhci->mem_service, MemoryService_C, super);

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
    if((entry->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_BULK ||
       (entry->flags & QH_FLAG_TYPE_MASK) == QH_FLAG_TYPE_CONTROL){
      uint32_t* i_time = (uint32_t*)uhci->qh_measurement->get_c(uhci->qh_measurement, entry);
      uint32_t transfer_duration;
      char* measure_msg;
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
      uhci->controller_logger->info_c(uhci->controller_logger, measure_msg, transfer_duration);
    }
    #endif

    callback(dev, S_TRANSFER, data);
    retransmission_occured = uhci->retransmission(uhci, entry, td);
    if (!retransmission_occured) {
      TD* rcvry = (TD*)uhci->qh_to_td_map->get_c(uhci->qh_to_td_map, entry);
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
    
    TD* rcvry = (TD*)uhci->qh_to_td_map->get_c(uhci->qh_to_td_map, entry);
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
void remove_transfer_entry(_UHCI *uhci, QH *entry) {
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

void destroy_transfer(_UHCI* uhci, UsbTransfer *transfer) {
  MemoryService_C* m = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);
  
  UsbTransaction* transaction = transfer->entry_transaction;

  while(transaction != (void*)0){
    UsbPacket* packet = transaction->entry_packet;
    UsbTransaction* temp = transaction->next;
    m->freeKernelMemory_c(m, packet, 0);
    m->freeKernelMemory_c(m, transaction, 0);

    transaction = temp;
  }

  m->freeKernelMemory_c(m, transfer, 0);
}

void runnable_function_uhci(UsbController* controller){
  _UHCI* uhci = container_of(controller, _UHCI, super);
  for(;;){
    if(uhci->signal){
      MemoryService_C* m = (MemoryService_C*)container_of(uhci->mem_service, MemoryService_C, super);

      for(QH* qh = uhci->qh_entry; qh != 0; qh = (QH*)(m->getVirtualAddress(m, qh->pyhsicalQHLP & QH_ADDRESS_MASK))){
        uhci->traverse_skeleton(uhci, qh);
      }
      if(!uhci->signal_not_override)
        uhci->signal = 0;
      else 
        uhci->signal_not_override = 0;  
    }
    else yield_c();
  } 
}

void handler_function_uhci(UsbController* controller) {
  _UHCI* uhci = container_of(controller, _UHCI, super);

  if(uhci->signal)
    uhci->signal_not_override = 1;
  uhci->signal = 1;
  // stop interrupt -> write clear
  Register* s_reg = uhci->look_for(uhci, Usb_Status);

  uint16_t lval = INT | ERR_INT;

  s_reg->write(s_reg, &lval);  
}