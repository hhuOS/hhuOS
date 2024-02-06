#include "MassStorageDriver.h"
#include "../../dev/UsbDevice.h"
#include "../../include/UsbControllerInclude.h"
#include "../../include/UsbErrors.h"
#include "../../include/UsbGeneral.h"
#include "../../include/UsbInterface.h"
#include "../../interfaces/TimeInterface.h"
#include "../../interfaces/LoggerInterface.h"
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
    Endpoint* endpoint = endpoints[i];
    if (!(endpoint->endpoint_desc.bmAttributes & TRANSFER_TYPE_BULK)) {
      continue;
    }
    uint8_t direction =
        endpoint->endpoint_desc.bEndpointAddress & DIRECTION_MASK;
    if (!internal_msd_driver->dev.bulk_in_endpoint_addr &&
        (direction == DIRECTION_IN)) {
      if(internal_msd_driver->dev.buffer == (void*)0){
        msd_buffer = (uint8_t *)mem_service->mapIO(
          mem_service, sizeof(uint8_t) * MAX_TRANSFER_BYTES, 1);
        internal_msd_driver->dev.buffer = msd_buffer;
      }    
      select = 1;
      internal_msd_driver->dev.bulk_in_endpoint_addr =
        endpoint->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;

    } else if (!internal_msd_driver->dev.bulk_out_endpoint_addr &&
               (direction == DIRECTION_OUT)) {
      if(internal_msd_driver->dev.buffer == (void*)0){
        msd_buffer = (uint8_t *)mem_service->mapIO(
          mem_service, sizeof(uint8_t) * MAX_TRANSFER_BYTES, 1);
          internal_msd_driver->dev.buffer = msd_buffer;
      }
      select = 1;
      internal_msd_driver->dev.bulk_out_endpoint_addr =
          endpoint->endpoint_desc.bEndpointAddress & ENDPOINT_MASK;
    }
  }

  if(select){
    internal_msd_driver->dev.usb_dev = dev;
    internal_msd_driver->dev.buffer_size = MAX_TRANSFER_BYTES;
    internal_msd_driver->dev.priority = PRIORITY_8;
    internal_msd_driver->dev.interface = interface;

    return 1;
  }

  return -1;
}

void new_mass_storage_driver(MassStorageDriver* driver, char* name, UsbDevice_ID* entry){
  driver->success_transfer = 0;
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
  driver->send_request_sense = &send_request_sense;
  driver->send_read_capacity__32_bit = &send_read_capacity__32_bit;
  driver->send_read_capacity__64_bit = &send_read_capacity__64_bit;
  driver->send_read = &send_read;
  driver->send_write = &send_write;
  driver->get_drive_size = &get_drive_size;
  driver->get_block_size = &get_block_size;
  driver->get_block_num  = &get_block_num;
  driver->test_mass_storage_writes = &test_mass_storage_writes;
  driver->init_sense_description = &init_sense_description;
  driver->is_valid_volume = &is_valid_volume;
  driver->get_inquiry_data = &get_inquiry_data;
  driver->get_capacity_descpritor = &get_capacity_descpritor;
  driver->get_sense_data = &get_sense_data;
  driver->get_capacity = &get_capacity;

  internal_msd_driver = driver;

  internal_msd_driver->super.new_usb_driver = &new_usb_driver;
  internal_msd_driver->super.new_usb_driver(&driver->super, name, entry);

  internal_msd_driver->init_logger = &init_msd_logger;
  internal_msd_driver->driver_logger = internal_msd_driver->init_logger(internal_msd_driver);
  internal_msd_driver->init_sense_description(internal_msd_driver);
  internal_msd_driver->parse_request_sense = &parse_request_sense;
}

Logger_C* init_msd_logger(MassStorageDriver* driver){
  Logger_C* driver_logger = (Logger_C*)interface_allocateMemory(sizeof(Logger_C), 0);
  driver_logger->new_logger = &new_logger;
  driver_logger->new_logger(driver_logger, USB_DRIVER_LOGGER_TYPE, LOGGER_LEVEL_INFO);
  
  return driver_logger;
}

void init_sense_description(MassStorageDriver* driver){
  driver->description_sense[RECOVERED_DATA_WITH_RETRIES] = "Recovered data with retries";
  driver->description_sense[RECOVERED_DATA_WITH_ECC] = "Recovered data with ECC";
  driver->description_sense[LOGICAL_DRIVE_NOT_READY_BECOMING_READY] = "Logical drive not ready - Becoming ready";
  driver->description_sense[LOGICAL_DRIVE_NOT_READY_INIT_REQUIRED] = "Logical drive not ready - intialization required";
  driver->description_sense[LOGICAL_UNIT_NOT_READY_FORMAT_IN_PROGRESS] = "Logical unit not ready - intialization required";
  driver->description_sense[LOGICAL_UNIT_NOT_READY_DEVICE_IS_BUSY] = "Logical unit not ready - format in progress";
  driver->description_sense[LOGICAL_UNIT_COMMUNICATION_ERROR] = "Logical unit communication error";
  driver->description_sense[LOGICAL_UNIT_COMMUNICATION_TIMEOUT] = "Logical unit communication time-out";
  driver->description_sense[LOGICAL_UNIT_COMMUNICATION_OVERRUN] = "Logical unit communication overrun";
  driver->description_sense[MEDIUM_NOT_PRESENT] = "Medium is not present";
  driver->description_sense[USB_TO_HOST_INTERFACE_FAILURE] = "USB-TO-HOST system interface failure";
  driver->description_sense[INSUFFICIENT_RESOURCES] = "Insufficient resources";
  driver->description_sense[UNKNOWN_ERR] = "Unknown error";
  driver->description_sense[NO_SEEK_COMPLETE] = "No seek complete";
  driver->description_sense[WRITE_FAULT] = "Write fault";
  driver->description_sense[ID_CRC_ERROR] = "ID CRC error";
  driver->description_sense[UNRECOVERED_READ_ERROR] = "Unrecovered read error";
  driver->description_sense[RECORDED_ENTITY_NOT_FOUND] = "Recorded Entity not found";
  driver->description_sense[UNKNOWN_FORMAT] = "Cannot read medium - unknown format";
  driver->description_sense[PARAMETER_LIST_LEN_ERR] = "Paramter list length error";
  driver->description_sense[INV_COMMAND_OPERATION_CODE] = "Invalid command operation code";
  driver->description_sense[LOGICAL_BLOCK_ADDR_OUT_OF_RANGE] = "Logical block address out of range";
  driver->description_sense[INV_FIELD_IN_COMMAND_PACKET] = "Invalid field in command packet";
  driver->description_sense[LOGICAL_UNIT_NOT_SUPPORTED] = "Logical unit not supported";
  driver->description_sense[INV_FIELD_IN_PARAMETER_LIST] = "Invalid field in paramter list";
  driver->description_sense[PARAMETER_NOT_SUPPORTED] = "Paramter not supported";
  driver->description_sense[PARAMETER_VALUE_INV] = "Parameter value invalid";
  driver->description_sense[MEDIA_CHANGED] = "Not ready to ready transition - Media changed";
  driver->description_sense[WRITE_PROT_MEDIA] = "Write protected media";
}

void disconnect_mass_storage(UsbDev* dev, Interface* interface){

}

void callback_config(UsbDev *dev, uint32_t status, void *data) {
  if(status == E_TRANSFER){
    internal_msd_driver->success_transfer = 0;
  }
  else if(status == S_TRANSFER){
    internal_msd_driver->success_transfer = 1;
  }
}

void new_storage_volume(struct MassStorageVolume* volume, uint8_t volume_num){
  volume->version = 0;
  volume->volume_number = volume_num;
  volume->volume_size = 0;
  volume->block_size = 0;
  volume->block_num = 0;
  volume->found_capacities = 0;
}

int configure_device(MassStorageDriver *driver) {
  UsbDev *dev = driver->dev.usb_dev;
  Interface *itf = driver->dev.interface;

  MemoryService_C *mem_service =
      (MemoryService_C *)container_of(dev->mem_service, MemoryService_C, super);
  uint8_t *command = (uint8_t *)mem_service->mapIO(
      mem_service, PAGE_SIZE * sizeof(uint8_t), 1);
  if(dev->get_max_logic_unit_numbers(dev, itf, command, &callback_config) == -1)
    return -1;
  uint32_t initial_time = getSystemTimeInMilli();
  uint32_t timeout = addMilis(initial_time, 50);
  uint32_t current_time;
  do{
    current_time = getSystemTimeInMilli();
  } while((current_time < timeout) && (driver->success_transfer == 0));

  if(!driver->success_transfer)
    return -1;

  unsigned int volumes = command[0];

  driver->volumes = volumes;
  driver->mass_storage_volumes = (MassStorageVolume*)mem_service->allocateKernelMemory_c(mem_service, sizeof(MassStorageVolume) * (command[0] + 1), 0);
  for(int i = 0; i < volumes + 1; i++){
    driver->mass_storage_volumes[i].new_storage_volume = &new_storage_volume;
    driver->mass_storage_volumes[i].new_storage_volume(driver->mass_storage_volumes + i, i);
  }
  CommandBlockWrapper* cbw = (CommandBlockWrapper*)command;
  CommandStatusWrapper* csw = (CommandStatusWrapper*)(command + sizeof(CommandBlockWrapper));
  RequestSense* rs = (RequestSense*)(command + sizeof(CommandBlockWrapper) + sizeof(CommandStatusWrapper));
  unsigned int add_offset = sizeof(CommandBlockWrapper) + sizeof(CommandStatusWrapper) + sizeof(RequestSense);

  for(int i = 0; i < volumes + 1; i++){
    InquiryCommandData* inquiry_data = (InquiryCommandData*)(command + add_offset);
    if(driver->send_inquiry(driver, cbw, csw, inquiry_data, i, rs) == -1)
      return -1;
    CapacityListHeader* clh = (CapacityListHeader*)(command + add_offset);  
    if(driver->send_read_format_capacities(driver, cbw, csw, clh, i, rs) == -1)
      return -1;  
    ReadCapacity__32_Bit* rc_32_bit = (ReadCapacity__32_Bit*)(command + add_offset);
    if(send_read_capacity__32_bit(driver, cbw, csw, rc_32_bit, i, rs) == -1)
      return -1;
    if(rc_32_bit->logical_block_address == 0xFFFFFFFF){
      // couldn't store with 32-bit
      ReadCapacity__64_Bit* rc_64_bit = (ReadCapacity__64_Bit*)(command + add_offset);
      if(send_read_capacity__64_bit(driver, cbw, csw, rc_64_bit, i, rs) == -1)
        return -1;
      driver->mass_storage_volumes[i].version = READ_CAPACITY_16;
      driver->mass_storage_volumes[i].rc_64_bit = *rc_64_bit;
    }
    else{
      driver->mass_storage_volumes[i].version = READ_CAPACITY_10;
      driver->mass_storage_volumes[i].rc_32_bit = *rc_32_bit;
    }
    // set volume size
    driver->mass_storage_volumes[i].volume_size = driver->get_drive_size(driver, i);
    driver->mass_storage_volumes[i].block_size  = driver->get_block_size(driver, i);
    driver->mass_storage_volumes[i].block_num   = driver->get_block_num(driver, i);
    #ifdef TEST_ON
    if(driver->test_mass_storage_writes(driver, cbw, csw, i, rs) == -1)
      return -1;
    #endif
    // reads 64 * block len as initial transfer
    if(send_read(driver, cbw, csw, driver->dev.buffer, 64, i, 0, 0, &callback_config, 
      BULK_INITIAL_STATE, rs) == -1)
      return -1;
  }

  mem_service->unmap(mem_service, (uint32_t)(uintptr_t)command);

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

uint64_t read(MassStorageDriver* driver, uint64_t start_lba, uint32_t blocks,
              uint8_t volume){
  
}

uint64_t write(){
  
}

int is_valid_volume(MassStorageDriver* driver, uint8_t volume_input){
  return (volume_input < driver->volumes ? 1 : 0);
}

int get_capacity_count(MassStorageDriver* driver, uint8_t volume){
  return driver->mass_storage_volumes[volume].found_capacities;
}

int send_inquiry(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                 CommandStatusWrapper *csw, InquiryCommandData *inquiry_data,
                 uint8_t volume, RequestSense* rs) {
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000024;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 6;

  driver->command_helper(driver, cbw->command, INQUIRY, 0, 0, 0, 0x24, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume, command_len);
  if(driver->send_command(driver, cbw, &callback_config) == -1)
    return -1;
  if(driver->get_data(driver, inquiry_data, sizeof(InquiryCommandData), BULK_INITIAL_STATE) == -1)
    return -1;
  if(driver->retrieve_status(driver, csw) == -1){
    driver->send_request_sense(driver, cbw, csw, rs, volume);
    return -1;
  }

  driver->mass_storage_volumes[volume].inquiry = *inquiry_data;

  return 1;
}

// copy len bytes to target buffer : for each get function

int get_inquiry_data(MassStorageDriver* driver, uint8_t volume, uint8_t param, 
                          uint8_t* target, uint8_t* len){
  InquiryCommandData inquiry_data = driver->mass_storage_volumes[volume].inquiry;
  
  if(param == PERI_QUALIFIER){
    *target = (inquiry_data.byte1 & PERI_QUALIFIER) >> 5;
    *len    = 1;
  }
  else if(param == PERI_DEVICE_TYPE){
    *target = inquiry_data.byte1 & PERI_DEVICE_TYPE;
    *len    = 1;
  }
  else if(param == RMB){
    *target = (inquiry_data.byte2 & RMB) >> 7;
    *len    = 1;
  }
  else if(param == VERSION){
    *target = inquiry_data.byte3;
    *len    = 1;
  }
  else if(param == NORM_ACA){
    *target = (inquiry_data.byte4 & NORM_ACA) >> 5;
    *len    = 1;
  }
  else if(param == HI_SUP){
    *target = (inquiry_data.byte4 & HI_SUP) >> 4;
    *len    = 1;
  }
  else if(param == RESPONSE_DATA_FORMAT){
    *target = inquiry_data.byte4 & RESPONSE_DATA_FORMAT;
    *len    = 1;
  }
  else if(param == ADDITIONAL_LEN){
    *target = inquiry_data.byte5 & ADDITIONAL_LEN;
    *len    = 1;
  }
  else if(param == SCCS){
    *target = (inquiry_data.byte6 & SCCS) >> 7;
    *len    = 1;
  }
  else if(param == ACC){
    *target = (inquiry_data.byte6 & ACC) >> 6;
    *len    = 1;
  }
  else if(param == TPGS){
    *target = (inquiry_data.byte6 & TPGS) >> 4;
    *len    = 1;
  }
  else if(param == THREE_PC){
    *target = (inquiry_data.byte6 & THREE_PC) >> 3;
    *len    = 1;
  }
  else if(param == PROT){
    *target = inquiry_data.byte6 & PROT;
    *len    = 1;
  }
  else if(param == RESV){
    *target = (inquiry_data.byte7 & RESV) >> 7;
    *len    = 1;
  }
  else if(param == ENC_SERV){
    *target = (inquiry_data.byte7 & ENC_SERV) >> 6;
    *len    = 1;
  }
  else if(param == VS){
    *target = (inquiry_data.byte7 & VS) >> 5;
    *len    = 1;
  }
  else if(param == MULTI_P){
    *target = (inquiry_data.byte7 & MULTI_P) >> 4;
    *len    = 1;
  }
  else if(param == ADDR_16){
    *target = inquiry_data.byte7 & ADDR_16;
    *len    = 1;
  }
  else if(param == WBUS_16){
    *target = (inquiry_data.byte8 & WBUS_16) >> 5;
    *len    = 1;
  }
  else if(param == SYNC_INQUIRY){
    *target = (inquiry_data.byte8 & SYNC_INQUIRY) >> 4;
    *len    = 1;
  }
  else if(param == CMDN_QUE){
    *target = (inquiry_data.byte8 & CMDN_QUE) >> 1;
    *len    = 1;
  }
  // copy data from kernel to target buffer, todo !
  else if(param == VENDOR_INFORMATION){
    target = inquiry_data.vendor_information;
    *len    = 8;
  }
  else if(param == PRODUCT_INFORMATION){
    target = inquiry_data.product_information;
    *len    = 16;
  }
  else if(param == PRODUCT_REVISION_LEVEL){
    target = inquiry_data.product_revision_level;
    *len    = 4;
  }
  else return -1;

  return 1;                    
}

int get_capacity_descpritor(MassStorageDriver* driver, uint8_t volume, uint8_t capacity_num, uint8_t param,
                                 uint8_t* target, uint8_t* len){
  if(capacity_num >= driver->mass_storage_volumes[volume].found_capacities) return -1;
  CapacityDescriptor c_desc = driver->mass_storage_volumes[volume].capacity_desc[capacity_num];

  if(param == NUMBER_OF_BLOCKS){
    uint32_t* extended_target = (uint32_t*)target;
    *extended_target = c_desc.number_of_blocks;
    *len    = 4;
  }
  else if(param == DESCRIPTOR_CODE){
    *target = c_desc.code;
    *len    = 1;
  }
  else if(param == BLOCK_LEN){
    uint32_t* total = (uint32_t*)target;
    *total = c_desc.block_length_b1 | c_desc.block_length_b2 << 8 | c_desc.block_length_b3 << 16;
    *len    = 4;
  }
  else return -1;

  return 1;
}

int get_sense_data(MassStorageDriver* driver, uint8_t volume, uint8_t param,
                        uint8_t* target, uint8_t* len){
  return -1;
}

int get_capacity(MassStorageDriver* driver, uint8_t volume, uint8_t param, 
                      uint8_t* target, uint8_t* len){
  if(driver->mass_storage_volumes[volume].version == READ_CAPACITY_10){
    ReadCapacity__32_Bit rc = driver->mass_storage_volumes[volume].rc_32_bit;
    if(param == LOGICAL_BLOCK_ADDRESS_32_BIT){
      uint32_t* block_address = (uint32_t*)target;
      *block_address = rc.logical_block_address;
      *len    = 4;
    }
    else if(param == BLOCK_SIZE_IN_BYTES_32_BIT){
      uint32_t* block_size = (uint32_t*)target;
      *block_size = rc.block_size;
      *len    = 4;
    }
    else return -1;

    return 1;
  }
  else if(driver->mass_storage_volumes[volume].version == READ_CAPACITY_16){
    ReadCapacity__64_Bit rc = driver->mass_storage_volumes[volume].rc_64_bit;
    if(param == LOGICAL_BLOCK_ADDRESS_64_BIT){
      uint32_t* logical_block_addr = (uint32_t*)target;
      *logical_block_addr = rc.lower_logical_block_address;
      *(logical_block_addr + 1) = rc.upper_logical_block_address;

      *len = 8;
    }
    else if(param == BLOCK_SIZE_IN_BYTES_64_BIT){
      uint32_t* block_size = (uint32_t*)target;
      *block_size = rc.block_size;
      *len = 4;
    }
    else if(param == P_TYPE){
      *target = (rc.byte1 & P_TYPE) >> 1;
      *len    = 1;
    }
    else if(param == PROT_EN){
      *target = rc.byte1 & PROT_EN;
      *len    = 1;
    }
    else if(param == P_I_EXPONENT){
      *target = (rc.byte2 & P_I_EXPONENT) >> 4;
      *len    = 1;
    }
    else if(param == LBS){
      *target = rc.byte2 & LBS;
      *len    = 1;
    }
    else if(param == LOWEST_BLOCK_ADDRESS){
      uint16_t* l_block_addr = (uint16_t*)target;
      *l_block_addr = rc.word1;
      *len    = 2;
    }
    else return -1;

    return 1;
  }

  return -1;
}

int send_read_format_capacities(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                                CommandStatusWrapper* csw, CapacityListHeader* clh,
                                uint8_t volume, RequestSense* rs) {
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
    if(driver->get_data(driver, clh, sizeof(CapacityListHeader), BULK_INITIAL_STATE) == -1)
      return -1;
    if(driver->retrieve_status(driver, csw))
      break;
    current_iteration++;  
  }

  // not functional if it enters
  if(current_iteration == READ_FORMAT_CAPACITIES_THRESHHOLD)
    return -1;
  
  driver->parse_format_capacities(driver, clh, volume);

  return 1;
}

void parse_format_capacities(MassStorageDriver* driver, CapacityListHeader* clh, uint8_t volume){
  uint8_t capacity_len = clh->capacity_len;
  uint8_t* capacity_descriptors = clh->capacity_descriptors;
  uint8_t current_capacity = 0;
  int curret_index = 0;
  while(current_capacity < capacity_len){
    CapacityDescriptor* c_desc = (CapacityDescriptor*)capacity_descriptors;
    driver->mass_storage_volumes[volume].capacity_desc[curret_index++] = *c_desc;
    current_capacity += sizeof(CapacityDescriptor);
  }

  driver->mass_storage_volumes[volume].found_capacities = curret_index;
}

int send_request_sense(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                       RequestSense* rs, uint8_t volume){
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000012;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 6;

  driver->command_helper(driver, cbw->command, REQUEST_SENSE, 0, 0, 0, 0x12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume, command_len);
  
  if(driver->send_command(driver, cbw, &callback_config) == -1)
    return -1;
  if(driver->get_data(driver, rs, sizeof(RequestSense), BULK_INITIAL_STATE) == -1)
    return -1;  
  if(driver->retrieve_status(driver, csw) == -1)
    return -1;  

  driver->parse_request_sense(driver, rs);

  return 1;
}

void parse_request_sense(MassStorageDriver* driver, RequestSense* rs){
  uint8_t error = rs->byte_1 & ERROR_CODE;
  uint8_t sense_key = rs->byte_2 & SENSE_KEY;
  uint8_t asc = rs->asc;
  uint8_t ascq = rs->ascq;

  uint8_t status;
  char* status_msg = "Error %u code : %u ; %s";
  if(sense_key == 0x01 && asc == 0x17 && ascq == 0x01)
    status = RECOVERED_DATA_WITH_RETRIES;
  if(sense_key == 0x01 && asc == 0x18 && ascq == 0x00)
    status = RECOVERED_DATA_WITH_ECC;
  if(sense_key == 0x02){
    if(asc == 0x04){
      if(ascq == 0x01)
        status = LOGICAL_DRIVE_NOT_READY_BECOMING_READY;
      if(ascq == 0x02)
        status = LOGICAL_DRIVE_NOT_READY_INIT_REQUIRED;
      if(ascq == 0x04)
        status = LOGICAL_UNIT_NOT_READY_FORMAT_IN_PROGRESS;
      if(ascq == 0xFF)
        status = LOGICAL_UNIT_NOT_READY_DEVICE_IS_BUSY;          
    }
    if(asc == 0x08){
      if(ascq == 0x00)
        status = LOGICAL_UNIT_COMMUNICATION_ERROR;
      if(ascq == 0x01)
        status = LOGICAL_UNIT_COMMUNICATION_TIMEOUT;
      if(ascq == 0x80)
        status = LOGICAL_UNIT_COMMUNICATION_OVERRUN;  
    }
    if(asc == 0x3A && ascq == 0x00)
      status = MEDIUM_NOT_PRESENT;
    if(asc == 0x54 && ascq == 0x00)
      status = USB_TO_HOST_INTERFACE_FAILURE;
    if(asc == 0x80 && ascq == 0x00)
      status = INSUFFICIENT_RESOURCES;
    if(asc == 0xFF && ascq == 0xFF)
      status = UNKNOWN_ERR;   
  }
  if(sense_key == 0x03){
    if(asc == 0x02 && ascq == 0x00)
      status = NO_SEEK_COMPLETE;
    if(asc == 0x03 && ascq == 0x00)
      status = WRITE_FAULT;
    if(asc == 0x10 && ascq == 0x00)
      status = ID_CRC_ERROR;
    if(asc == 0x11 && ascq == 0x00)
      status = UNRECOVERED_READ_ERROR;
    if(asc == 0x14 && ascq == 0x00)
      status = RECORDED_ENTITY_NOT_FOUND;
    if(asc == 0x30 && ascq == 0x01)
      status = UNKNOWN_FORMAT;       
  }
  if(sense_key == 0x05){
    if(asc == 0x1A && ascq == 0x00)
      status = PARAMETER_LIST_LEN_ERR;
    if(asc == 0x20 && ascq == 0x00)
      status = INV_COMMAND_OPERATION_CODE;
    if(asc == 0x21 && ascq == 0x00)
      status = LOGICAL_BLOCK_ADDR_OUT_OF_RANGE;
    if(asc == 0x24 && ascq == 0x00)
      status = INV_FIELD_IN_COMMAND_PACKET;
    if(asc == 0x25 && ascq == 0x00)
      status = LOGICAL_UNIT_NOT_SUPPORTED;
    if(asc == 0x26 && ascq == 0x00)
      status = INV_FIELD_IN_PARAMETER_LIST;
    if(asc == 0x26 && ascq == 0x01)
      status = PARAMETER_NOT_SUPPORTED;
    if(asc == 0x26 && ascq == 0x02)
      status = PARAMETER_VALUE_INV;
  }
  if(sense_key == 0x06)
    if(asc == 0x28 && ascq == 0x00)
      status = MEDIA_CHANGED;
  if(sense_key == 0x07)
    if(asc == 0x27 && ascq == 0x00)
      status = WRITE_PROT_MEDIA;
  driver->driver_logger->info_c(driver->driver_logger, status_msg, error, status, driver->description_sense[status]);            
}

// query 32_bit first -> if 32_bit is too low make a call to 64 bit -> save flag in driver
int send_read_capacity__32_bit(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                               ReadCapacity__32_Bit* rc, uint8_t volume, RequestSense* rs){
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000008;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 10;

  driver->command_helper(driver, cbw->command, READ_CAPACITY_10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume, command_len);

  if(driver->send_command(driver, cbw, &callback_config) == -1)
    return -1;
  if(driver->get_data(driver, rc, sizeof(ReadCapacity__32_Bit), BULK_INITIAL_STATE) == -1)
    return -1;
  if(driver->retrieve_status(driver, csw) == -1){
    driver->send_request_sense(driver, cbw, csw, rs, volume);
    return -1;
  }

  return 1;      
}

int send_read_capacity__64_bit(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                               ReadCapacity__64_Bit* rc, uint8_t volume, RequestSense* rs){
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000020;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 16;

  driver->command_helper(driver, cbw->command, READ_CAPACITY_16, 0x10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume, command_len);

  if(driver->send_command(driver, cbw, &callback_config) == -1)
    return -1;
  if(driver->get_data(driver, rc, sizeof(ReadCapacity__64_Bit), BULK_INITIAL_STATE) == -1)
    return -1;
  if(driver->retrieve_status(driver, csw) == -1){
    driver->send_request_sense(driver, cbw, csw, rs, volume);
    return -1;
  }

  return 1;       
}

int send_read(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
              uint8_t* buffer, uint32_t blocks, uint8_t volume, uint32_t lba_low, uint32_t lba_high,
              callback_function callback, uint8_t flags, RequestSense* rs){
  MassStorageVolume mass_storage_volume = driver->mass_storage_volumes[volume];
  uint32_t millis = getSystemTimeInMilli();
  uint8_t inner_flags = FLAGS_IN;
  // reminder big-endian when larger than 1 byte
  uint8_t block_b1 = blocks & 0xFF, block_b2 = (blocks & 0xFF00) >> 8, block_b3 = (blocks & 0xFF0000) >> 16, block_b4 = (blocks & 0xFF000000) >> 24;
  uint8_t command_len;
  if(mass_storage_volume.version == READ_CAPACITY_10){
    command_len = 12;
    uint8_t lba_b1 = lba_low & 0xFF, lba_b2 = (lba_low & 0xFF00) >> 8, lba_b3 = (lba_low & 0xFF0000) >> 16, lba_b4 = (lba_low & 0xFF000000) >> 24;

    driver->command_helper(driver, cbw->command, READ_10, 0, lba_b4, lba_b3, lba_b2, lba_b1, 0, block_b2,
                           block_b1, 0, 0, 0, 0, 0, 0, 0);
  }
  else if(mass_storage_volume.version == READ_CAPACITY_16){
    command_len = 16;
    uint8_t lba_b1 = lba_low & 0xFF, lba_b2 = (lba_low & 0xFF00) >> 8, lba_b3 = (lba_low & 0xFF0000) >> 16, lba_b4 = (lba_low & 0xFF000000) >> 24;
    uint8_t lba_b5 = lba_high & 0xFF, lba_b6 = (lba_high & 0xFF00) >> 8, lba_b7 = (lba_high & 0xFF0000) >> 16, lba_b8 = (lba_high & 0xFF000000) >> 24;
  
    driver->command_helper(driver, cbw->command, READ_16, 0, lba_b8, lba_b7, lba_b6, lba_b5,
                           lba_b4, lba_b3, lba_b2, lba_b1, block_b4, block_b3, block_b2, block_b1, 0, 0);
  }
  CapacityDescriptor cap_desc = driver->mass_storage_volumes[volume].capacity_desc[0];
  uint32_t b1 = cap_desc.block_length_b1, b2 = cap_desc.block_length_b2, b3 = cap_desc.block_length_b3;

  uint32_t block_len = b1 | b2 << 8 | b3 << 16;

  uint32_t t_len = block_len * blocks;

  driver->build_command(driver, cbw, millis, t_len, inner_flags, volume, command_len);

  if(driver->send_command(driver, cbw, callback) == -1)
    return -1;
  if(driver->get_data(driver, buffer, t_len, flags) == -1)
    return -1;
  if(driver->retrieve_status(driver, csw) == -1){
    driver->send_request_sense(driver, cbw, csw, rs, volume);
    return -1;        
  }

  return 1;
}

int send_write(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
               uint8_t* buffer, uint32_t blocks, uint8_t volume, uint32_t lba_low, uint32_t lba_high,
               callback_function callback, uint8_t flags, RequestSense* rs){
  MassStorageVolume mass_storage_volume = driver->mass_storage_volumes[volume];
  uint32_t millis = getSystemTimeInMilli();
  uint8_t inner_flags = FLAGS_OUT;
  // reminder big-endian when larger than 1 byte
  uint8_t block_b1 = blocks & 0xFF, block_b2 = (blocks & 0xFF00) >> 8, block_b3 = (blocks & 0xFF0000) >> 16, block_b4 = (blocks & 0xFF000000) >> 24;
  uint8_t command_len;
  if(mass_storage_volume.version == READ_CAPACITY_10){
    command_len = 12;
    uint8_t lba_b1 = lba_low & 0xFF, lba_b2 = (lba_low & 0xFF00) >> 8, lba_b3 = (lba_low & 0xFF0000) >> 16, lba_b4 = (lba_low & 0xFF000000) >> 24;

    driver->command_helper(driver, cbw->command, WRITE_10, 0, lba_b4, lba_b3, lba_b2, lba_b1, 0, block_b2,
                          block_b1, 0, 0, 0, 0, 0, 0, 0);
  }
  else if(mass_storage_volume.version == READ_CAPACITY_16){
    command_len = 16;
    uint8_t lba_b1 = lba_low & 0xFF, lba_b2 = (lba_low & 0xFF00) >> 8, lba_b3 = (lba_low & 0xFF0000) >> 16, lba_b4 = (lba_low & 0xFF000000) >> 24;
    uint8_t lba_b5 = lba_high & 0xFF, lba_b6 = (lba_high & 0xFF00) >> 8, lba_b7 = (lba_high & 0xFF0000) >> 16, lba_b8 = (lba_high & 0xFF000000) >> 24;
  
    driver->command_helper(driver, cbw->command, WRITE_16, 0, lba_b8, lba_b7, lba_b6, lba_b5,
                           lba_b4, lba_b3, lba_b2, lba_b1, block_b4, block_b3, block_b2, block_b1, 0, 0);
  }
  CapacityDescriptor cap_desc = driver->mass_storage_volumes[volume].capacity_desc[0];
  uint32_t b1 = cap_desc.block_length_b1, b2 = cap_desc.block_length_b2, b3 = cap_desc.block_length_b3;

  uint32_t block_len = b1 | b2 << 8 | b3 << 16;

  uint32_t t_len = block_len * blocks;

  driver->build_command(driver, cbw, millis, t_len, inner_flags, volume, command_len);

  if(driver->send_command(driver, cbw, callback) == -1)
    return -1;
  if(driver->send_data(driver, buffer, t_len, flags) == -1)
    return -1;
  if(driver->retrieve_status(driver, csw) == -1){
    driver->send_request_sense(driver, cbw, csw, rs, volume);
    return -1;        
  }

  return 1;
}

uint32_t get_drive_size(MassStorageDriver* driver, uint8_t volume){
  if(driver->mass_storage_volumes[volume].found_capacities == 0) return 0;

  CapacityDescriptor cap_desc = driver->mass_storage_volumes[volume].capacity_desc[0];
  uint32_t block_len = driver->get_block_size(driver, volume);
  uint32_t num_of_blocks = cap_desc.number_of_blocks;

  return block_len * num_of_blocks;
}

uint32_t get_block_size(MassStorageDriver* driver, uint8_t volume){
  if(driver->mass_storage_volumes[volume].found_capacities == 0) return 0;

  CapacityDescriptor cap_desc = driver->mass_storage_volumes[volume].capacity_desc[0];
  uint32_t b1 = cap_desc.block_length_b1, b2 = cap_desc.block_length_b2, b3 = cap_desc.block_length_b3;

  uint32_t block_len = b1 | b2 << 8 | b3 << 16;

  return block_len; 
}

uint32_t get_block_num(MassStorageDriver* driver, uint8_t volume){
  if(driver->mass_storage_volumes[volume].found_capacities == 0) return 0;

  CapacityDescriptor cap_desc = driver->mass_storage_volumes[volume].capacity_desc[0];

  return cap_desc.number_of_blocks;
}

int get_data(MassStorageDriver *driver, void *data, unsigned int len, uint8_t flags) {
  UsbDev *dev = driver->dev.usb_dev;
  Interface *itf = driver->dev.interface;
  unsigned int endpoint = driver->dev.bulk_in_endpoint_addr;
  unsigned int pipe = usb_rcvbulkpipe(endpoint);
  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, data, len, &callback_config,
                    flags);
  if(!driver->success_transfer)
    return -1;
  return 1;
}

int send_data(MassStorageDriver *driver, void *data, unsigned int len, uint8_t flags) {
  UsbDev* dev = driver->dev.usb_dev;
  Interface* itf = driver->dev.interface;
  unsigned int endpoint = driver->dev.bulk_out_endpoint_addr;
  unsigned int pipe = usb_sndbulkpipe(endpoint);
  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, data, len, &callback_config, flags);
  if(!driver->success_transfer)
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
  if (!driver->success_transfer)
    return -1;
  return 1;
}

int retrieve_status(MassStorageDriver *driver, CommandStatusWrapper *csw) {
  UsbDev *dev = driver->dev.usb_dev;
  Interface *itf = driver->dev.interface;
  unsigned int pipe = usb_rcvbulkpipe(driver->dev.bulk_in_endpoint_addr);

  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, csw, CSW_LEN, &callback_config,
                    BULK_INITIAL_STATE);

  if (!driver->success_transfer){
    return -1;
  }

  if(csw->status != 0 || csw->data_residue > 0)
    return -1;
  
  return 1;  
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

int test_mass_storage_writes(MassStorageDriver* driver, CommandBlockWrapper* cbw, 
                             CommandStatusWrapper* csw, uint8_t volume, RequestSense* rs){
  uint32_t block_size = driver->get_block_size(driver, volume);
  uint8_t* buffer = driver->dev.buffer;
  // write 1 block starting from lba 0
  for(int i = 0; i < block_size; i++){
    buffer[i] = 'A';
  }
  driver->driver_logger->info_c(driver->driver_logger, "sending %u bytes to dev, starting at lba %u", block_size, 0);
  driver->send_write(driver, cbw, csw, buffer, 1, volume, 0, 0, &callback_config, BULK_INITIAL_STATE, rs);
  // write 4 blocks starting from lba 1
  for(int i = 0; i < (4 * block_size); i++){
    buffer[i] = 'B';
  }
  driver->driver_logger->info_c(driver->driver_logger, "sending %u bytes to dev, starting at lba %u", block_size * 4, 1);
  driver->send_write(driver, cbw, csw, buffer, 4, volume, 1, 0, &callback_config, BULK_INITIAL_STATE, rs);
  // write 1 block starting from lba 5
  for(int i = 0; i < block_size; i++){
    buffer[i] = 'C';
  }
  driver->driver_logger->info_c(driver->driver_logger, "sending %u bytes to dev, starting at lba %u", block_size, 5);
  driver->send_write(driver, cbw, csw, buffer, 1, volume, 5, 0, &callback_config, BULK_INITIAL_STATE, rs);
  // write 50 blocks starting
  for(int i = 0; i < 50 * block_size; i++){
    buffer[i] = 'D';
  }
  driver->driver_logger->info_c(driver->driver_logger, "sending %u bytes to dev, starting at lba %u", block_size * 50, 6);                 
  driver->send_write(driver, cbw, csw, buffer, 50, volume, 6, 0, &callback_config, BULK_INITIAL_STATE, rs);            

  driver->driver_logger->info_c(driver->driver_logger, "recevice %u bytes from dev, started at lba %u", block_size * 56, 0);
  driver->send_read(driver, cbw, csw, buffer, 56, volume, 0, 0, &callback_config, BULK_INITIAL_STATE, rs);

  driver->driver_logger->info_c(driver->driver_logger, "starting tests ... ");
  // assert
  int test_case = 1;
  int test_failed = 0;
  int test_one_failed = 0;
  for(int i = 0; i < block_size; i++){
    if(buffer[i] != 'A'){
      test_failed = 1;
      test_one_failed = 1;
      driver->driver_logger->info_c(driver->driver_logger, "test %d failed", test_case);
      break;
    }
  }
  if(!test_failed)
    driver->driver_logger->info_c(driver->driver_logger, "test %d passed", test_case);
  test_case++;
  test_failed = 0;    
  for(int i = block_size; i < block_size + (4 * block_size); i++){
    if(buffer[i] != 'B'){
      test_failed = 1;
      test_one_failed = 1;
      driver->driver_logger->info_c(driver->driver_logger, "test %d failed", test_case);
      break;
    }
  }
  if(!test_failed)
    driver->driver_logger->info_c(driver->driver_logger, "test %d passed", test_case);
  test_case++;  
  test_failed = 0;    
  for(int i = (5 * block_size); i < (6 * block_size); i++){
    if(buffer[i] != 'C'){
      test_failed = 1;
      test_one_failed = 1;
      driver->driver_logger->info_c(driver->driver_logger, "test %d failed", test_case);
      break;
    }
  }
  if(!test_failed)
    driver->driver_logger->info_c(driver->driver_logger, "test %d passed", test_case);
  test_case++;
  test_failed = 0;    
  for(int i = (6 * block_size); i < (56 * block_size); i++){
    if(buffer[i] != 'D'){
      test_failed = 1;
      test_one_failed = 1;
      driver->driver_logger->info_c(driver->driver_logger, "test %d failed", test_case);
      break;
    }
  }
  if(!test_failed)
    driver->driver_logger->info_c(driver->driver_logger, "test %d passed", test_case);
  test_case++;  
  test_failed = 0;    

  if(!test_one_failed)
    driver->driver_logger->info_c(driver->driver_logger, "all tests passed - device is working properly ...");
  else
    driver->driver_logger->info_c(driver->driver_logger, "not all tests passed - device is not working properly ...");
  return 1;
}