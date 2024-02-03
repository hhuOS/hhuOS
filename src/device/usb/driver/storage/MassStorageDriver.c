#include "MassStorageDriver.h"
#include "../../../../lib/util/io/key/InputEvents.h"
#include "../../dev/UsbDevice.h"
#include "../../events/EventDispatcher.h"
#include "../../events/event/Event.h"
#include "../../events/event/hid/KeyBoardEvent.h"
#include "../../include/UsbControllerInclude.h"
#include "../../include/UsbErrors.h"
#include "../../include/UsbGeneral.h"
#include "../../interfaces/TimeInterface.h"
#include "../UsbDriver.h"
#include "CommandInterface.h"

static MassStorageDriver *internal_msd_driver = 0;

static void new_usb_driver(UsbDriver *usb_driver, char *name,
                           UsbDevice_ID *entry);

static void new_usb_driver(UsbDriver *usb_driver, char *name,
                           UsbDevice_ID *entry) {
  usb_driver->probe = &probe_mass_storage;
  usb_driver->disconnect = &disconnect_mass_storage;

  usb_driver->name = name;
  usb_driver->entry = entry;
  usb_driver->head.l_e = 0;
  usb_driver->l_e.l_e = 0;
}

int16_t probe_mass_storage(UsbDev *dev, Interface *interface) {

  Endpoint **endpoints = interface->active_interface->endpoints;
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;
  // check if we should support that interface !!!
  int e = interface_desc.bNumEndpoints;
  uint8_t* msd_buffer;
  uint8_t select = 0;
  MemoryService_C *mem_service =
      (MemoryService_C *)container_of(dev->mem_service, MemoryService_C, super);

  for (int i = 0; i < e; i++) {
    if (!(endpoints[i]->endpoint_desc.bmAttributes & TRANSFER_TYPE_BULK)) {
      continue;
    }
    uint8_t direction =
        endpoints[i]->endpoint_desc.bEndpointAddress & DIRECTION_MASK;
    if (!internal_msd_driver->dev.bulk_in_endpoint_addr &&
        (direction == DIRECTION_IN)) {
      if(internal_msd_driver->dev.buffer == (void*)0){
        msd_buffer = (uint8_t *)mem_service->mapIO(
          mem_service, sizeof(uint8_t) * MAX_TRANSFER_BYTES, 1);
      }    
      select = 1;
      internal_msd_driver->dev.bulk_in_endpoint_addr =
          endpoints[i]->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;

    } else if (!internal_msd_driver->dev.bulk_out_endpoint_addr &&
               (direction == DIRECTION_OUT)) {
      if(internal_msd_driver->dev.buffer == (void*)0){
        msd_buffer = (uint8_t *)mem_service->mapIO(
          mem_service, sizeof(uint8_t) * MAX_TRANSFER_BYTES, 1);
      }
      select = 1;
      internal_msd_driver->dev.bulk_out_endpoint_addr =
          endpoints[i]->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;
    }
  }

  if(select){
    internal_msd_driver->dev.usb_dev = dev;
    internal_msd_driver->dev.buffer = msd_buffer;
    internal_msd_driver->dev.buffer_size = MAX_TRANSFER_BYTES;
    internal_msd_driver->dev.priority = PRIORITY_8;
    internal_msd_driver->dev.interface = interface;

    return 1;
  }

  return -1;
}

void new_mass_storage_driver(MassStorageDriver* driver, char* name, UsbDevice_ID* entry){
  driver->invalid_transfer = 0;
  driver->found_capacities = 0;
  driver->dev.usb_dev = 0;
  driver->dev.bulk_in_endpoint_addr = 0;
  driver->dev.bulk_out_endpoint_addr = 0;
  driver->dev.buffer = 0;
  driver->dev.buffer_size = 0;
  driver->dev.priority = 0;
  driver->dev.interface = 0;
  driver->dev.usb_driver = (UsbDriver*)driver;
  driver->dev.callback = &callback_mass_storage;

  driver->configure_device = &configure_device;
  driver->read_command = &read_command;
  driver->send_inquiry = &send_inquiry;
  driver->send_read_format_capacities = &send_read_format_capacities;
  driver->parse_format_capacities = &parse_format_capacities;
  driver->get_data = &get_data;
  driver->send_data = &send_data;
  driver->build_command = &build_command;
  driver->send_command = &send_command;
  driver->retrieve_status = &retrieve_status;
  driver->check_csw_status = &check_csw_status;
  driver->command_helper = &command_helper;

  internal_msd_driver = driver;

  internal_msd_driver->super.new_usb_driver = &new_usb_driver;
  internal_msd_driver->super.new_usb_driver(&driver->super, name, entry);
}

void disconnect_mass_storage(UsbDev* dev, Interface* interface){

}

void callback_config(UsbDev *dev, uint32_t status, void *data) {
  if(status == E_TRANSFER){
    internal_msd_driver->invalid_transfer = 1;
  }
  else if(status == S_TRANSFER){
    internal_msd_driver->invalid_transfer = 0;
  }
}

int configure_device(MassStorageDriver *driver) {
  UsbDev *dev = driver->dev.usb_dev;
  Interface *itf = driver->dev.interface;

  MemoryService_C *mem_service =
      (MemoryService_C *)container_of(dev->mem_service, MemoryService_C, super);
  uint8_t *command = (uint8_t *)mem_service->mapIO(
      mem_service, PAGE_SIZE * sizeof(uint8_t), 1);
  if(dev->get_max_logic_unit_numbers(dev, itf, command) == -1)
    return -1;
  driver->volumes = command[0];  
  CommandBlockWrapper* cbw = (CommandBlockWrapper*)command;
  CommandStatusWrapper* csw = (CommandStatusWrapper*)(command + sizeof(CommandBlockWrapper));
  unsigned int add_offset = sizeof(CommandBlockWrapper) + sizeof(CommandStatusWrapper);

  InquiryCommandData* inquiry_data = (InquiryCommandData*)(command + add_offset);
  if(driver->send_inquiry(driver, cbw, csw, inquiry_data, 0) == -1)
    return -1;
  CapacityListHeader* clh = (CapacityListHeader*)(command + add_offset);  
  if(driver->send_read_format_capacities(driver, cbw, csw, clh, 0) == -1)
    return -1;  

  mem_service->unmap(mem_service, command);

  return 1;      
}

void callback_mass_storage(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;
}

void callback_cbw(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;
}

void callback_csw(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;
}

int read_command(MassStorageDriver* driver, CommandCode cc, void* data){
  return 0;
}

int send_inquiry(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                 CommandStatusWrapper *csw, InquiryCommandData *inquiry_data,
                 uint8_t volume) {
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000024;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 6;

  driver->command_helper(driver, cbw->command, INQUIRY, 0, 0, 0, 0x24, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume, command_len);
  if(driver->send_command(driver, cbw, &callback_config) == -1)
    return -1;
  if(driver->get_data(driver, inquiry_data, sizeof(InquiryCommandData)) == -1)
    return -1;
  if(driver->retrieve_status(driver, csw) == -1)
    return -1;

  driver->inquiry = *inquiry_data;

  return 1;
}

int send_read_format_capacities(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                                CommandStatusWrapper* csw, CapacityListHeader* clh,
                                uint8_t volume) {
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x000000FC;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 10;

  driver->command_helper(driver, cbw->command, READ_FORMAT_CAPACITIES, 0, 0, 0, 0, 0, 0, 0, 0xFC, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume, command_len);
  int current_iteration = 0;
  while(current_iteration < READ_FORMAT_CAPACITIES_THRESHHOLD){
    if(driver->send_command(driver, cbw, &callback_config) == -1)
      return -1;
    if(driver->get_data(driver, clh, sizeof(CapacityListHeader)) == -1)
      return -1;
    if(driver->retrieve_status(driver, csw))
      break;
    current_iteration++;  
  }

  // not functional if it enters
  if(current_iteration == READ_FORMAT_CAPACITIES_THRESHHOLD)
    return -1;
  
  driver->parse_format_capacities(driver, clh);

  return 1;
}

void parse_format_capacities(MassStorageDriver* driver, CapacityListHeader* clh){
  uint8_t capacity_len = clh->capacity_len;
  uint8_t* capacity_descriptors = clh->capacity_descriptors;
  uint8_t current_capacity = 0;
  int curret_index = 0;
  while(current_capacity < capacity_len){
    CapacityDescriptor* c_desc = (CapacityDescriptor*)capacity_descriptors;
    driver->capacity_desc[curret_index++] = *c_desc;
    current_capacity += sizeof(CapacityDescriptor);
  }
}

int get_data(MassStorageDriver *driver, void *data, unsigned int len) {
  UsbDev *dev = driver->dev.usb_dev;
  Interface *itf = driver->dev.interface;
  unsigned int endpoint = driver->dev.bulk_in_endpoint_addr;
  unsigned int pipe = usb_rcvbulkpipe(endpoint);
  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, data, len, &callback_config,
                    BULK_INITIAL_STATE);
  if(driver->invalid_transfer)
    return -1;
  return 1;
}

int send_data(MassStorageDriver *driver, void *data, unsigned int len) {
  UsbDev* dev = driver->dev.usb_dev;
  Interface* itf = driver->dev.interface;
  unsigned int endpoint = driver->dev.bulk_out_endpoint_addr;
  unsigned int pipe = usb_sndbulkpipe(endpoint);
  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, data, len, &callback_config, BULK_INITIAL_STATE);
  if(driver->invalid_transfer)
    return -1;
  return 1;  
}

void build_command(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                   uint32_t tag, uint32_t transfer_length, uint8_t flags,
                   uint8_t lun, uint8_t command_len) {
  cbw->signature = CBW_SIGNATURE;
  cbw->tag = tag;
  cbw->transfer_length = transfer_length;
  cbw->flags = flags;
  cbw->lun = lun;
  cbw->command_len = command_len;
}

int send_command(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                 callback_function callback) {
  if (cbw->flags != FLAGS_IN && cbw->flags != FLAGS_OUT)
    return -1;
  if (cbw->lun > 15)
    return -1;
  if (cbw->command_len > COMMAND_LEN)
    return -1;
  uint8_t valid_command = cbw->command[0];
  if (valid_command != REQUEST_SENSE && valid_command != INQUIRY &&
      valid_command != READ_FORMAT_CAPACITIES &&
      valid_command != READ_CAPACITY_10 && valid_command != READ_10 &&
      valid_command != WRITE_10 && valid_command != READ_16 &&
      valid_command != WRITE_16 && valid_command != READ_CAPACITY_16 &&
      valid_command != REPORT_LUNS && valid_command != READ_12 &&
      valid_command != WRITE_12)
    return -1;
  UsbDev *dev = driver->dev.usb_dev;
  Interface *itf = driver->dev.interface;
  unsigned int pipe = usb_sndbulkpipe(driver->dev.bulk_out_endpoint_addr);
  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, cbw, CBW_LEN, &callback_config,
                    BULK_INITIAL_STATE);
  if (driver->invalid_transfer)
    return -1;
  return 1;
}

int retrieve_status(MassStorageDriver *driver, CommandStatusWrapper *csw) {
  UsbDev *dev = driver->dev.usb_dev;
  Interface *itf = driver->dev.interface;
  unsigned int pipe = usb_rcvbulkpipe(driver->dev.bulk_in_endpoint_addr);

  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, csw, CSW_LEN, &callback_config,
                    BULK_INITIAL_STATE);

  if (driver->invalid_transfer)
    return -1;
}

uint8_t check_csw_status(MassStorageDriver *driver, CommandStatusWrapper *csw) {
  uint8_t status = csw->status;

  return status;
}

void command_helper(MassStorageDriver *driver, uint8_t *command, uint8_t c1,
                    uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6,
                    uint8_t c7, uint8_t c8, uint8_t c9, uint8_t c10,
                    uint8_t c11, uint8_t c12, uint8_t c13, uint8_t c14,
                    uint8_t c15, uint8_t c16) {
  command[0] = c1;
  command[1] = c2;
  command[2] = c3;
  command[3] = c4;
  command[4] = c5;
  command[5] = c6;
  command[6] = c7;
  command[7] = c8;
  command[8] = c9;
  command[9] = c10;
  command[10] = c11;
  command[11] = c12;
  command[12] = c13;
  command[13] = c14;
  command[14] = c15;
  command[15] = c16;
}