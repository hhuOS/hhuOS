#include "MassStorageDriver.h"
#include "../../dev/UsbDevice.h"
#include "../../include/UsbControllerInclude.h"
#include "../../include/UsbErrors.h"
#include "../../include/UsbGeneral.h"
#include "../../include/UsbInterface.h"
#include "../../interfaces/LoggerInterface.h"
#include "../../interfaces/MapInterface.h"
#include "../../interfaces/TimeInterface.h"
#include "../../utility/Utils.h"
#include "../UsbDriver.h"
#include "CommandInterface.h"

static int16_t probe_mass_storage(UsbDev *dev, Interface *interface);
static void callback_mass_storage(UsbDev *dev, uint32_t status, void *data);
static void disconnect_mass_storage(UsbDev* dev, Interface* interface);
static int configure_device(MassStorageDriver *driver);
static int send_inquiry(MassStorageDriver *driver, MassStorageDev* msd_dev, CommandBlockWrapper *cbw,
                 CommandStatusWrapper *csw, InquiryCommandData *inquiry_data,
                 uint8_t volume, RequestSense* rs);
static int send_read_format_capacities(MassStorageDriver *driver, MassStorageDev* msd_dev, CommandBlockWrapper *cbw,
                                CommandStatusWrapper* csw, CapacityListHeader* clh,
                                uint8_t volume, RequestSense* rs);
static void parse_format_capacities(MassStorageDriver* driver, MassStorageDev* msd_dev, CapacityListHeader* clh, uint8_t volume);
static int get_data(MassStorageDriver *driver, MassStorageDev* msd_dev, void *data, unsigned int len, uint8_t flags, callback_function callback);                       
static int send_data(MassStorageDriver *driver, MassStorageDev* msd_dev, void *data, unsigned int len, uint8_t flags, callback_function callback);
static void build_command(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                   uint32_t tag, uint32_t transfer_length, uint8_t flags,
                   uint8_t lun, uint8_t command_len);
static int send_command(MassStorageDriver *driver, MassStorageDev* msd_dev, CommandBlockWrapper *cbw,
                 callback_function callback, uint8_t flags);  
static int retrieve_status(MassStorageDriver *driver, MassStorageDev* msd_dev, CommandStatusWrapper *csw, uint8_t flags, callback_function callback);
static uint8_t check_csw_status(MassStorageDriver *driver, CommandStatusWrapper *csw);
static void command_helper(MassStorageDriver *driver, uint8_t *command, uint8_t c1,
                    uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6,
                    uint8_t c7, uint8_t c8, uint8_t c9, uint8_t c10,
                    uint8_t c11, uint8_t c12, uint8_t c13, uint8_t c14,
                    uint8_t c15, uint8_t c16);
static int send_request_sense(MassStorageDriver* driver, MassStorageDev* msd_dev, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                       RequestSense* rs, uint8_t volume);
static int send_read_capacity__32_bit(MassStorageDriver* driver, MassStorageDev* msd_dev, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                               ReadCapacity__32_Bit* rc, uint8_t volume, RequestSense* rs);
static int send_read_capacity__64_bit(MassStorageDriver* driver, MassStorageDev* msd_dev, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                               ReadCapacity__64_Bit* rc, uint8_t volume, RequestSense* rs);                                                                          
static int send_read(MassStorageDriver* driver, MassStorageDev* msd_dev, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
              uint8_t* buffer, uint32_t blocks, uint8_t volume, uint32_t lba_low, uint32_t lba_high,
              callback_function callback, uint8_t flags, RequestSense* rs);
static int send_write(MassStorageDriver* driver, MassStorageDev* msd_dev, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
               uint8_t* buffer, uint32_t blocks, uint8_t volume, uint32_t lba_low, uint32_t lba_high,
               callback_function callback, uint8_t flags, RequestSense* rs);              
static uint32_t get_drive_size(MassStorageDriver* driver, MassStorageDev* msd_dev, uint8_t volume);
static uint32_t get_block_size(MassStorageDriver* driver, MassStorageDev* msd_dev, uint8_t volume);
static uint32_t get_block_num(MassStorageDriver* driver, MassStorageDev* msd_dev, uint8_t volume);
static int test_mass_storage_writes(MassStorageDriver* driver, MassStorageDev* msd_dev, 
                            CommandBlockWrapper* cbw, CommandStatusWrapper* csw, uint8_t volume, RequestSense* rs);
static void parse_request_sense(MassStorageDriver* driver, RequestSense* rs);
static void init_sense_description(MassStorageDriver* driver);
static int is_valid_volume(MassStorageDriver* driver, MassStorageDev* msd_dev, uint8_t volume);
static int get_capacity_count(MassStorageDriver* driver, MassStorageDev* msd_dev, uint8_t volume);
static int get_inquiry_data(MassStorageDriver* driver, MassStorageDev* msd_dev, uint8_t volume, uint8_t param, 
                          uint8_t* target, uint8_t* len);
static int get_capacity_descpritor(MassStorageDriver* driver, MassStorageDev* msd_dev, uint8_t volume, uint8_t capacity_num, uint8_t param,
                                 uint8_t* target, uint8_t* len);
static int get_sense_data(MassStorageDriver* driver, uint8_t volume, uint8_t param,
                        uint8_t* target, uint8_t* len);
static int get_capacity(MassStorageDriver* driver, MassStorageDev* msd_dev, uint8_t volume, uint8_t param, 
                      uint8_t* target, uint8_t* len);
static void set_dev_properites(struct MassStorageDriver* driver, MassStorageDev* msd_dev, UsbDev* dev, Interface* interface);
static void msd_match_routine(struct MassStorageDriver* driver, MemoryService_C* mem_service, 
  UsbDev* dev, Endpoint* endpoint, struct MassStorageDev* msd_dev, uint8_t* select, 
  unsigned int* msd_endpoint);
static CommandBlockWrapper* get_free_cbw(MassStorageDriver *driver);
static void free_cbw(MassStorageDriver* driver, CommandBlockWrapper* cbw);
static CommandStatusWrapper* get_free_csw(MassStorageDriver* driver);
static void free_csw(MassStorageDriver* driver, CommandStatusWrapper* csw);
static void init_msd_maps(MassStorageDriver* driver);
static uint64_t read_msd(MassStorageDriver* driver, uint8_t* target, uint64_t start_lba, uint32_t blocks,
              uint16_t magic_number, uint8_t u_tag, uint8_t volume, uint8_t minor);
static int set_callback_msd(MassStorageDriver* driver, msd_callback callback, 
                      uint16_t magic_number, uint8_t u_tag);
static int unset_callback_msd(MassStorageDriver* driver, uint16_t magic_number, uint8_t u_tag);
static uint64_t write_msd(MassStorageDriver* driver, uint8_t* source, uint64_t start_lba, uint32_t blocks,
                uint16_t magic_number, uint8_t u_tag, uint8_t volume, uint8_t minor);
static int init_io_msd(MassStorageDriver* driver, MassStorageDev* msd_dev, CommandBlockWrapper* cbw, uint32_t blocks,
                uint64_t start_lba, uint8_t* buffer, uint16_t magic_number, uint8_t u_tag,
                uint8_t volume, uint8_t read);
static void callback_mass_storage(UsbDev *dev, uint32_t status, void *data);
static void callback_cbw_data_read(UsbDev *dev, uint32_t status, void *data);
static void callback_cbw_data_write(UsbDev* dev, uint32_t status, void* data);
static void callback_csw(UsbDev *dev, uint32_t status, void *data);
static uint32_t calc_t_len(MassStorageDriver* driver, MassStorageDev* msd, uint8_t volume, uint32_t blocks);
static void build_read_command(MassStorageDriver* driver, MassStorageDev* msd, CommandBlockWrapper* cbw, uint8_t volume, 
                        uint32_t blocks, uint32_t lba_low, uint32_t lba_high, uint32_t t_len);
static void build_write_command(MassStorageDriver* driver, MassStorageDev* msd, CommandBlockWrapper* cbw, uint8_t volume,
                        uint32_t blocks, uint32_t lba_low, uint32_t lba_high, uint32_t t_len);
static void clear_msd_map(MassStorageDriver* driver, uint32_t id);
static MassStorageDev* get_free_msd_dev(MassStorageDriver* driver);
static void free_msd_dev(MassStorageDriver* driver, MassStorageDev* msd_dev);
static MassStorageDev* match_msd_dev(MassStorageDriver* driver, UsbDev* dev);
static MassStorageDev* get_msd_dev_by_minor(MassStorageDriver* driver, uint8_t minor);
static uint8_t valid_block_number(MassStorageDriver* driver, uint32_t blocks);
static void copy_from_user(uint8_t* __user, uint8_t* __kernel, size_t size);
static void copy_to_user(uint8_t* __user, uint8_t* __kernel, size_t size);
static void new_storage_volume(struct MassStorageVolume* volume, uint8_t volume_num);

static MassStorageDriver *internal_msd_driver = 0;

static int16_t probe_mass_storage(UsbDev *dev, Interface *interface) {
  Endpoint **endpoints = interface->active_interface->endpoints;
  InterfaceDescriptor interface_desc =
      interface->active_interface->alternate_interface_desc;
  int e = interface_desc.bNumEndpoints;
  uint8_t select = 0;
  MemoryService_C *mem_service = __DEV_MEMORY(dev);

  __REQUEST_OR_LEAVE__(MassStorageDev, internal_msd_driver, 
    get_free_msd_dev, msd_dev);

  for (int i = 0; i < e; i++) {
    Endpoint *endpoint = endpoints[i];
    __DRIVER_TYPE_BULK__(__STRUCT_CALL__(internal_msd_driver, free_msd_dev, msd_dev),
      dev, endpoint);
    if (!msd_dev->bulk_in_endpoint_addr && __STRUCT_CALL__(dev, 
      __is_direction_in, endpoint)) {
      __STRUCT_CALL__(internal_msd_driver, msd_match_routine, mem_service, dev, 
        endpoint, msd_dev, &select, &msd_dev->bulk_in_endpoint_addr);
    } 
    else if (!msd_dev->bulk_out_endpoint_addr && __STRUCT_CALL__(dev, 
      __is_direction_out, endpoint)) {
      __STRUCT_CALL__(internal_msd_driver, msd_match_routine, mem_service, dev, 
        endpoint, msd_dev, &select, &msd_dev->bulk_out_endpoint_addr);
    }
  }
  __IF_CUSTOM__(select, __STRUCT_CALL__(internal_msd_driver, set_dev_properites, 
    msd_dev, dev, interface);
    return __RET_S__);

  return __RET_E__;
}

void new_mass_storage_driver(MassStorageDriver *driver, char *name,
                             UsbDevice_ID *entry) {
  for (int i = 0; i < MAX_DEVICES_PER_USB_DRIVER; i++) {
    driver->msd_map[i] = 0;
    driver->dev[i].success_transfer = 0;
    driver->dev[i].mass_storage_volumes = 0;
    driver->dev[i].volumes = 0;
    driver->dev[i].usb_dev = 0;
    driver->dev[i].bulk_in_endpoint_addr = 0;
    driver->dev[i].bulk_out_endpoint_addr = 0;
    driver->dev[i].buffer = 0;
    driver->dev[i].buffer_size = 0;
    driver->dev[i].priority = 0;
    driver->dev[i].interface = 0;
    driver->dev[i].usb_driver = (UsbDriver *)driver;
  }
  internal_msd_driver = driver;
  
  __INIT_MSD_DRIVER__(driver, name, entry);
  __STRUCT_CALL__(driver, init_sense_description);
  __STRUCT_CALL__(driver, init_msd_maps);
}

static MassStorageDev *get_msd_dev_by_minor(MassStorageDriver *driver, uint8_t minor) {
  __IF_RET_NULL__(minor >= MAX_DEVICES_PER_USB_DRIVER);
  return driver->dev + minor;
}

static void msd_match_routine(MassStorageDriver* driver, MemoryService_C* mem_service, 
  UsbDev* dev, Endpoint* endpoint, MassStorageDev* msd_dev, uint8_t* select, 
  unsigned int* msd_endpoint){
  __IF_CUSTOM__(__IS_NULL__(msd_dev->buffer), msd_dev->buffer = 
    __MAP_IO_KERNEL__(mem_service, uint8_t, sizeof(uint8_t) * MAX_TRANSFER_BYTES));
  *select = 1;
  *msd_endpoint = __STRUCT_CALL__(dev, 
        __endpoint_number, endpoint);
}

static void set_dev_properites(MassStorageDriver *driver, MassStorageDev* msd_dev, 
  UsbDev* dev, Interface* interface) {
  MemoryService_C *m = __DEV_MEMORY(dev);
  driver->csw_map_io = (uint8_t *)m->mapIO(m, PAGE_SIZE, 1);
  driver->cbw_map_io = (uint8_t *)m->mapIO(m, PAGE_SIZE, 1);

  msd_dev->usb_dev = dev;
  msd_dev->buffer_size = MAX_TRANSFER_BYTES;
  msd_dev->priority = PRIORITY_8;
  msd_dev->interface = interface;

  __mem_set(driver->csw_map_io, PAGE_SIZE, 0);
  __mem_set(driver->cbw_map_io, PAGE_SIZE, 0);

  __mem_set(driver->csw_map_io_bitmap, PAGE_SIZE / sizeof(CommandStatusWrapper),
          0);
  __mem_set(driver->cbw_map_io_bitmap, PAGE_SIZE / sizeof(CommandBlockWrapper),
          0);
}

static void init_sense_description(MassStorageDriver *driver) {
  driver->description_sense[RECOVERED_DATA_WITH_RETRIES] =
      "Recovered data with retries";
  driver->description_sense[RECOVERED_DATA_WITH_ECC] =
      "Recovered data with ECC";
  driver->description_sense[LOGICAL_DRIVE_NOT_READY_BECOMING_READY] =
      "Logical drive not ready - Becoming ready";
  driver->description_sense[LOGICAL_DRIVE_NOT_READY_INIT_REQUIRED] =
      "Logical drive not ready - intialization required";
  driver->description_sense[LOGICAL_UNIT_NOT_READY_FORMAT_IN_PROGRESS] =
      "Logical unit not ready - intialization required";
  driver->description_sense[LOGICAL_UNIT_NOT_READY_DEVICE_IS_BUSY] =
      "Logical unit not ready - format in progress";
  driver->description_sense[LOGICAL_UNIT_COMMUNICATION_ERROR] =
      "Logical unit communication error";
  driver->description_sense[LOGICAL_UNIT_COMMUNICATION_TIMEOUT] =
      "Logical unit communication time-out";
  driver->description_sense[LOGICAL_UNIT_COMMUNICATION_OVERRUN] =
      "Logical unit communication overrun";
  driver->description_sense[MEDIUM_NOT_PRESENT] = "Medium is not present";
  driver->description_sense[USB_TO_HOST_INTERFACE_FAILURE] =
      "USB-TO-HOST system interface failure";
  driver->description_sense[INSUFFICIENT_RESOURCES] = "Insufficient resources";
  driver->description_sense[UNKNOWN_ERR] = "Unknown error";
  driver->description_sense[NO_SEEK_COMPLETE] = "No seek complete";
  driver->description_sense[WRITE_FAULT] = "Write fault";
  driver->description_sense[ID_CRC_ERROR] = "ID CRC error";
  driver->description_sense[UNRECOVERED_READ_ERROR] = "Unrecovered read error";
  driver->description_sense[RECORDED_ENTITY_NOT_FOUND] =
      "Recorded Entity not found";
  driver->description_sense[UNKNOWN_FORMAT] =
      "Cannot read medium - unknown format";
  driver->description_sense[PARAMETER_LIST_LEN_ERR] =
      "Paramter list length error";
  driver->description_sense[INV_COMMAND_OPERATION_CODE] =
      "Invalid command operation code";
  driver->description_sense[LOGICAL_BLOCK_ADDR_OUT_OF_RANGE] =
      "Logical block address out of range";
  driver->description_sense[INV_FIELD_IN_COMMAND_PACKET] =
      "Invalid field in command packet";
  driver->description_sense[LOGICAL_UNIT_NOT_SUPPORTED] =
      "Logical unit not supported";
  driver->description_sense[INV_FIELD_IN_PARAMETER_LIST] =
      "Invalid field in paramter list";
  driver->description_sense[PARAMETER_NOT_SUPPORTED] = "Paramter not supported";
  driver->description_sense[PARAMETER_VALUE_INV] = "Parameter value invalid";
  driver->description_sense[MEDIA_CHANGED] =
      "Not ready to ready transition - Media changed";
  driver->description_sense[WRITE_PROT_MEDIA] = "Write protected media";
}

static MassStorageDev *get_free_msd_dev(MassStorageDriver *driver) {
  __GET_FREE_DEV__(MassStorageDev, driver->dev, driver->msd_map);
}

static void free_msd_dev(MassStorageDriver *driver, MassStorageDev *msd_dev) {
  __FREE_DEV__(msd_dev, driver->dev, driver->msd_map);
}

static MassStorageDev *match_msd_dev(MassStorageDriver *driver, UsbDev *dev) {
  __MATCH_DEV__(MassStorageDev, driver->dev, usb_dev, dev);
}

static void disconnect_mass_storage(UsbDev *dev, Interface *interface) {}

static void callback_config(UsbDev *dev, uint32_t status, void *data) {
  MassStorageDev *msd_dev =
      internal_msd_driver->match_msd_dev(internal_msd_driver, dev);

  if (msd_dev == (void *)0)
    return;

  if (status == E_TRANSFER) {
    msd_dev->success_transfer = 0;
  } else if (status == S_TRANSFER) {
    msd_dev->success_transfer = 1;
  }
}

static void new_storage_volume(struct MassStorageVolume *volume, uint8_t volume_num) {
  volume->version = 0;
  volume->volume_number = volume_num;
  volume->volume_size = 0;
  volume->block_size = 0;
  volume->block_num = 0;
  volume->found_capacities = 0;
}

static void init_msd_maps(MassStorageDriver *driver) {
  CommandBlockWrapper_Int_Map *c_map = (CommandBlockWrapper_Int_Map *)
    interface_allocateMemory(sizeof(CommandBlockWrapper_Int_Map), 0);
  CommandStatusWrapper_Int_Map *s_map = (CommandStatusWrapper_Int_Map *)
    interface_allocateMemory(sizeof(CommandStatusWrapper_Int_Map), 0);
  Data_Int_Map *d_map =
      (Data_Int_Map *)interface_allocateMemory(sizeof(Data_Int_Map), 0);
  Int_Callback_Map *i_callback_map =
      (Int_Callback_Map *)interface_allocateMemory(sizeof(Int_Callback_Map), 0);
  Int_Buffer_Map *i_buffer_map =
      (Int_Buffer_Map *)interface_allocateMemory(sizeof(Int_Buffer_Map), 0);
  Int_T_Len_Map *i_tlen_map =
      (Int_T_Len_Map *)interface_allocateMemory(sizeof(Int_T_Len_Map), 0);
  Int_Mem_Buffer_Map *i_mem_buffer_map =
      (Int_Mem_Buffer_Map *)interface_allocateMemory(sizeof(Int_Mem_Buffer_Map), 0);
  __STRUCT_INIT__(c_map, new_map, newCommandBlockIntMap, 
    "Map<CommandBlockWrapper*,uint32_t*>");
  __STRUCT_INIT__(s_map, new_map, newCommandStatusIntMap,
    "Map<CommandStatusWraper*,uin32_t*>");
  __STRUCT_INIT__(d_map, new_map, newDataIntMap, "Map<uint8_t*,uint32_t*>");
  __STRUCT_INIT__(i_callback_map, new_map, newIntCallbackMap, 
    "Map<uint32_t,callback>");
  __STRUCT_INIT__(i_buffer_map, new_map, newIntBufferMap, "Map<uint32_t, uint8_t*>");
  __STRUCT_INIT__(i_tlen_map, new_map, newIntTLenMap, "Map<uint32_t,uint32_t*>");
  __STRUCT_INIT__(i_mem_buffer_map, new_map, newIntMemBufferMap, 
    "Map<uint32_t, uint8_t*>");

  driver->cbw_map = (SuperMap *)c_map;
  driver->csw_map = (SuperMap *)s_map;
  driver->data_map = (SuperMap *)d_map;
  driver->callback_map = (SuperMap *)i_callback_map;
  driver->stored_target_map = (SuperMap *)i_buffer_map;
  driver->stored_len_map = (SuperMap *)i_tlen_map;
  driver->stored_mem_buffer_map = (SuperMap *)i_mem_buffer_map;
}

static int configure_device(MassStorageDriver *driver) {
  for (int j = 0; j < MAX_DEVICES_PER_USB_DRIVER; j++) {
    if (driver->msd_map[j] == 0)
      continue;
    UsbDev *dev = driver->dev[j].usb_dev;
    Interface *itf = driver->dev[j].interface;

    MassStorageDev *msd_dev = driver->dev + j;

    MemoryService_C *mem_service = __DEV_MEMORY(dev);
    uint8_t *command = (uint8_t *)mem_service->mapIO(
        mem_service, PAGE_SIZE * sizeof(uint8_t), 1);
    if (dev->get_max_logic_unit_numbers(dev, itf, command, &callback_config) ==
        -1)
      return -1;

    if (!msd_dev->success_transfer)
      return -1;

    unsigned int volumes = command[0];
    msd_dev->volumes = volumes;
    msd_dev->mass_storage_volumes =
        (MassStorageVolume *)mem_service->allocateKernelMemory_c(
            mem_service, sizeof(MassStorageVolume) * (command[0] + 1), 0);

    for (int i = 0; i < volumes + 1; i++) {
      msd_dev->mass_storage_volumes[i].new_storage_volume = &new_storage_volume;
      msd_dev->mass_storage_volumes[i].new_storage_volume(
          msd_dev->mass_storage_volumes + i, i);
    }

    CommandBlockWrapper *cbw = (CommandBlockWrapper *)command;
    CommandStatusWrapper *csw =
        (CommandStatusWrapper *)(command + sizeof(CommandBlockWrapper));
    RequestSense *rs = (RequestSense *)(command + sizeof(CommandBlockWrapper) +
                                        sizeof(CommandStatusWrapper));
    unsigned int add_offset = sizeof(CommandBlockWrapper) +
                              sizeof(CommandStatusWrapper) +
                              sizeof(RequestSense);

    for (int i = 0; i < volumes + 1; i++) {
      InquiryCommandData *inquiry_data =
          (InquiryCommandData *)(command + add_offset);
      if (driver->send_inquiry(driver, msd_dev, cbw, csw, inquiry_data, i,
                               rs) == -1)
        return -1;
      CapacityListHeader *clh = (CapacityListHeader *)(command + add_offset);
      if (driver->send_read_format_capacities(driver, msd_dev, cbw, csw, clh, i,
                                              rs) == -1)
        return -1;
      ReadCapacity__32_Bit *rc_32_bit =
          (ReadCapacity__32_Bit *)(command + add_offset);
      if (driver->send_read_capacity__32_bit(driver, msd_dev, cbw, csw,
                                             rc_32_bit, i, rs) == -1)
        return -1;
      if (rc_32_bit->logical_block_address == 0xFFFFFFFF) {
        // couldn't store with 32-bit
        ReadCapacity__64_Bit *rc_64_bit =
            (ReadCapacity__64_Bit *)(command + add_offset);
        if (driver->send_read_capacity__64_bit(driver, msd_dev, cbw, csw,
                                               rc_64_bit, i, rs) == -1)
          return -1;
        msd_dev->mass_storage_volumes[i].version = READ_CAPACITY_16;
        msd_dev->mass_storage_volumes[i].rc_64_bit = *rc_64_bit;
      } else {
        msd_dev->mass_storage_volumes[i].version = READ_CAPACITY_10;
        msd_dev->mass_storage_volumes[i].rc_32_bit = *rc_32_bit;
      }
      // set volume size
      msd_dev->mass_storage_volumes[i].volume_size =
          driver->get_drive_size(driver, msd_dev, i);
      msd_dev->mass_storage_volumes[i].block_size =
          driver->get_block_size(driver, msd_dev, i);
      msd_dev->mass_storage_volumes[i].block_num =
          driver->get_block_num(driver, msd_dev, i);
#ifdef TEST_ON
      if (driver->test_mass_storage_writes(driver, msd_dev, cbw, csw, i, rs) ==
          -1)
        return -1;
#endif
      // reads 64 * block len as initial transfer
      if (driver->send_read(driver, msd_dev, cbw, csw, msd_dev->buffer, 64, i,
                            0, 0, &callback_config, BULK_INITIAL_STATE,
                            rs) == -1)
        return -1;
    }

    mem_service->unmap(mem_service, (uint32_t)(uintptr_t)command);
  }

  return 1;
}

static void callback_mass_storage(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;

  MemoryService_C *mem_service = __DEV_MEMORY(dev);
  CommandStatusWrapper *csw =
      internal_msd_driver->get_free_csw(internal_msd_driver);

  uint32_t *id = (uint32_t *)internal_msd_driver->data_map->get_c(
      internal_msd_driver->data_map, (uint8_t *)data);
  uint8_t *target = internal_msd_driver->stored_target_map->get_c(
      internal_msd_driver->stored_target_map, id);
  msd_callback callback = internal_msd_driver->callback_map->get_c(
      internal_msd_driver->callback_map, id);

  MassStorageDev *msd_dev =
      internal_msd_driver->match_msd_dev(internal_msd_driver, dev);

  if (csw == (void *)0 || msd_dev == (void *)0) {
    callback(target, 0);

    internal_msd_driver->clear_msd_map(internal_msd_driver, *id);
    mem_service->unmap(mem_service, (uint32_t)(uintptr_t)(uint8_t *)data);
    return;
  }

  if (internal_msd_driver->retrieve_status(internal_msd_driver, msd_dev, csw, 0,
                                           &callback_csw) == -1) {
    callback(target, 0);

    internal_msd_driver->clear_msd_map(internal_msd_driver, *id);
    mem_service->unmap(mem_service, (uint32_t)(uintptr_t)(uint8_t *)data);
    internal_msd_driver->free_csw(internal_msd_driver, csw);
    return;
  }
}

static void callback_cbw_data_write(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;
  uint32_t *id = (uint32_t *)internal_msd_driver->cbw_map->get_c(
      internal_msd_driver->cbw_map, (CommandBlockWrapper *)data);
  uint32_t *stored_len = internal_msd_driver->stored_len_map->get_c(
      internal_msd_driver->stored_len_map, id);
  uint8_t *target = internal_msd_driver->stored_target_map->get_c(
      internal_msd_driver->stored_target_map, id);
  msd_callback callback = internal_msd_driver->callback_map->get_c(
      internal_msd_driver->callback_map, id);

  MemoryService_C *m = __DEV_MEMORY(dev);
  uint8_t *mm = (uint8_t *)m->mapIO(m, sizeof(uint8_t) * *stored_len, 0);

  MassStorageDev *msd_dev =
      internal_msd_driver->match_msd_dev(internal_msd_driver, dev);

  if (mm == (void *)0 || msd_dev == (void *)0) {
    callback(target, 0);

    internal_msd_driver->free_cbw(internal_msd_driver,
                                  (CommandBlockWrapper *)data);
    internal_msd_driver->clear_msd_map(internal_msd_driver, *id);
    return;
  }

  copy_from_user(target, mm, *stored_len);

  internal_msd_driver->data_map->put_c(internal_msd_driver->data_map, mm, id);
  internal_msd_driver->stored_mem_buffer_map->put_c(
      internal_msd_driver->stored_mem_buffer_map, id, mm);

  if (internal_msd_driver->send_data(internal_msd_driver, msd_dev, mm,
                                     *stored_len, 0,
                                     &callback_mass_storage) == -1) {
    callback(target, 0);

    internal_msd_driver->data_map->remove_c(internal_msd_driver->data_map, mm);
    m->unmap(m, (uint32_t)(uintptr_t)mm);
    internal_msd_driver->clear_msd_map(internal_msd_driver, *id);
  }

  internal_msd_driver->free_cbw(internal_msd_driver,
                                (CommandBlockWrapper *)data);
}

static void callback_cbw_data_read(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;

  uint32_t *id = (uint32_t *)internal_msd_driver->cbw_map->get_c(
      internal_msd_driver->cbw_map, (CommandBlockWrapper *)data);
  uint32_t *stored_len = internal_msd_driver->stored_len_map->get_c(
      internal_msd_driver->stored_len_map, id);
  uint8_t *target = internal_msd_driver->stored_target_map->get_c(
      internal_msd_driver->stored_target_map, id);
  msd_callback callback = internal_msd_driver->callback_map->get_c(
      internal_msd_driver->callback_map, id);

  MemoryService_C *m = __DEV_MEMORY(dev);
  uint8_t *mm = (uint8_t *)m->mapIO(m, sizeof(uint8_t) * *stored_len, 0);

  MassStorageDev *msd_dev =
      internal_msd_driver->match_msd_dev(internal_msd_driver, dev);

  if (mm == (void *)0 || msd_dev == (void *)0) {
    callback(target, 0);

    internal_msd_driver->free_cbw(internal_msd_driver,
                                  (CommandBlockWrapper *)data);
    internal_msd_driver->clear_msd_map(internal_msd_driver, *id);
    return;
  }

  internal_msd_driver->data_map->put_c(internal_msd_driver->data_map, mm, id);
  internal_msd_driver->stored_mem_buffer_map->put_c(
      internal_msd_driver->stored_mem_buffer_map, id, mm);

  if (internal_msd_driver->get_data(internal_msd_driver, msd_dev, mm,
                                    *stored_len, 0,
                                    &callback_mass_storage) == -1) {
    callback(target, 0);

    internal_msd_driver->data_map->remove_c(internal_msd_driver->data_map, mm);
    m->unmap(m, (uint32_t)(uintptr_t)mm);
    internal_msd_driver->clear_msd_map(internal_msd_driver, *id);
  }

  internal_msd_driver->free_cbw(internal_msd_driver,
                                (CommandBlockWrapper *)data);
}

static void callback_csw(UsbDev *dev, uint32_t status, void *data) {
  if (status & E_TRANSFER)
    return;

  MemoryService_C *m = __DEV_MEMORY(dev);

  uint32_t *id = (uint32_t *)internal_msd_driver->csw_map->get_c(
      internal_msd_driver->csw_map, (CommandStatusWrapper *)data);
  uint8_t *target = internal_msd_driver->stored_target_map->get_c(
      internal_msd_driver->stored_target_map, id);
  msd_callback callback = internal_msd_driver->callback_map->get_c(
      internal_msd_driver->callback_map, id);
  uint32_t *len = internal_msd_driver->stored_len_map->get_c(
      internal_msd_driver->stored_len_map, id);
  uint8_t *mm = internal_msd_driver->stored_mem_buffer_map->get_c(
      internal_msd_driver->stored_mem_buffer_map, id);

  // if csw->status is errorneous or if data residue is greater than 0 we should
  // not fill target buffer
  if (((CommandStatusWrapper *)data)->status != 0 ||
      ((CommandStatusWrapper *)data)->data_residue > 0) {
    callback(target, 0);

    internal_msd_driver->free_csw(internal_msd_driver,
                                  (CommandStatusWrapper *)data);
    internal_msd_driver->clear_msd_map(internal_msd_driver, *id);
    m->unmap(m, (uint32_t)(uintptr_t)mm);
    return;
  }

  copy_to_user(target, mm, *len);

  m->unmap(m, (uint32_t)(uintptr_t)mm);

  callback(target, *len);
  internal_msd_driver->free_csw(internal_msd_driver,
                                (CommandStatusWrapper *)data);
  internal_msd_driver->clear_msd_map(internal_msd_driver, *id);
}

static void copy_from_user(uint8_t *__user, uint8_t *__kernel, size_t size) {
  // security checks are missing
  __mem_cpy(__user, __kernel, size);
}

static void copy_to_user(uint8_t *__user, uint8_t *__kernel, size_t size) {
  // security checks are missing
  __mem_cpy(__kernel, __user, size);
}

// clear stored_target entry, stored_len_entry and stored_mem_buffer_entry
static void clear_msd_map(MassStorageDriver *driver, uint32_t id) {
  if (driver->stored_target_map->contains_c(driver->stored_target_map, &id)) {
    driver->stored_target_map->remove_c(driver->stored_target_map, &id);
  }
  if (driver->stored_len_map->contains_c(driver->stored_len_map, &id)) {
    uint32_t *len = driver->stored_len_map->get_c(driver->stored_len_map, &id);
    interface_freeMemory(len, 0);
    driver->stored_len_map->remove_c(driver->stored_len_map, &id);
  }
  if (driver->stored_mem_buffer_map->contains_c(driver->stored_mem_buffer_map,
                                                &id)) {
    uint8_t *mem_buffer = driver->stored_mem_buffer_map->get_c(
        driver->stored_mem_buffer_map, &id);
    interface_freeMemory(mem_buffer, 0);
    driver->stored_mem_buffer_map->remove_c(driver->stored_mem_buffer_map, &id);
  }
}

static CommandBlockWrapper *get_free_cbw(MassStorageDriver *driver) {
  for (int i = 0; i < PAGE_SIZE / sizeof(CommandBlockWrapper); i++) {
    // dev->device_mutex->acquire_c(dev->device_mutex);
    if (driver->cbw_map_io_bitmap[i] == 0) {
      driver->cbw_map_io_bitmap[i] = 1;
      // dev->device_mutex->release_c(dev->device_mutex);
      return (CommandBlockWrapper *)(driver->cbw_map_io +
                                     (i * sizeof(CommandBlockWrapper)));
    }
    // dev->device_mutex->release_c(dev->device_mutex);
  }
  return (void *)0;
}

static void free_cbw(MassStorageDriver *driver, CommandBlockWrapper *cbw) {
  for (int i = 0; i < PAGE_SIZE; i += sizeof(CommandBlockWrapper)) {
    // dev->device_mutex->acquire_c(dev->device_mutex);
    if ((driver->cbw_map_io + i) == (uint8_t *)cbw) {
      driver->cbw_map_io_bitmap[i / sizeof(UsbDeviceRequest)] = 0;
      // dev->device_mutex->release_c(dev->device_mutex);
      return;
    }
    // dev->device_mutex->release_c(dev->device_mutex);
  }
}

static CommandStatusWrapper *get_free_csw(MassStorageDriver *driver) {
  for (int i = 0; i < PAGE_SIZE / sizeof(CommandStatusWrapper); i++) {
    // dev->device_mutex->acquire_c(dev->device_mutex);
    if (driver->csw_map_io_bitmap[i] == 0) {
      driver->csw_map_io_bitmap[i] = 1;
      // dev->device_mutex->release_c(dev->device_mutex);
      return (CommandStatusWrapper *)(driver->cbw_map_io +
                                      (i * sizeof(CommandStatusWrapper)));
    }
    // dev->device_mutex->release_c(dev->device_mutex);
  }
  return (void *)0;
}

static void free_csw(MassStorageDriver *driver, CommandStatusWrapper *csw) {
  for (int i = 0; i < PAGE_SIZE; i += sizeof(CommandBlockWrapper)) {
    // dev->device_mutex->acquire_c(dev->device_mutex);
    if ((driver->csw_map_io + i) == (uint8_t *)csw) {
      driver->csw_map_io_bitmap[i / sizeof(CommandStatusWrapper)] = 0;
      // dev->device_mutex->release_c(dev->device_mutex);
      return;
    }
    // dev->device_mutex->release_c(dev->device_mutex);
  }
}

static int set_callback_msd(MassStorageDriver *driver, msd_callback callback,
                     uint16_t magic_number, uint8_t u_tag) {
  uint32_t map_entry = magic_number | u_tag << 16;

  if (driver->callback_map->contains_c(driver->callback_map, &map_entry)) {
    return -1;
  }
  uint32_t *wrapper_entry =
      (uint32_t *)interface_allocateMemory(sizeof(uint32_t), 0);
  if (wrapper_entry == (void *)0)
    return -1;

  *wrapper_entry = map_entry;

  driver->callback_map->put_c(driver->callback_map, wrapper_entry, callback);

  return 1;
}

static int unset_callback_msd(MassStorageDriver *driver, uint16_t magic_number,
                       uint8_t u_tag) {
  uint32_t map_entry = magic_number | u_tag << 16;

  if (!driver->callback_map->contains_c(driver->callback_map, &map_entry))
    return -1;
  driver->callback_map->remove_c(driver->callback_map, &map_entry);

  return 1;
}

// magic_number an u_tag have to match set_callback magic_number and u_tag !!
static uint64_t read_msd(MassStorageDriver *driver, uint8_t *buffer,
                  uint64_t start_lba, uint32_t blocks, uint16_t magic_number,
                  uint8_t u_tag, uint8_t volume, uint8_t minor) {

  MassStorageDev *msd_dev = driver->dev + minor;

  if (msd_dev == (void *)0)
    return 0;
  if (!driver->is_valid_volume(driver, msd_dev, volume))
    return 0;
  if (!driver->valid_block_number(driver, blocks))
    return 0;

  CommandBlockWrapper *cbw = driver->get_free_cbw(driver);

  if (cbw == (void *)0)
    return 0;

  uint32_t id = magic_number | u_tag << 16;

  if (!driver->init_io_msd(driver, msd_dev, cbw, blocks, start_lba, buffer,
                           magic_number, u_tag, volume, 1)) {
    driver->free_cbw(driver, cbw);
    goto read_fail;
  }

  if (driver->send_command(driver, msd_dev, cbw, &callback_cbw_data_read, 0) ==
      -1) {
    goto read_fail;
  }

  goto read_success;

read_fail:
  msd_callback callback =
      driver->callback_map->get_c(driver->callback_map, &id);
  uint8_t *target =
      driver->stored_target_map->get_c(driver->stored_target_map, &id);
  callback(target, 0);
  driver->clear_msd_map(driver, id);
  return 0;

read_success:
  return 1;
}

static uint8_t valid_block_number(MassStorageDriver *driver, uint32_t blocks) {
  return blocks <= MAX_BLOCKS;
}

static uint64_t write_msd(MassStorageDriver *driver, uint8_t *buffer,
                   uint64_t start_lba, uint32_t blocks, uint16_t magic_number,
                   uint8_t u_tag, uint8_t volume, uint8_t minor) {

  MassStorageDev *msd_dev = driver->dev + minor;

  if (msd_dev == (void *)0)
    return 0;
  if (!driver->is_valid_volume(driver, msd_dev, volume))
    return 0;
  if (!driver->valid_block_number(driver, blocks))
    return 0;

  CommandBlockWrapper *cbw = driver->get_free_cbw(driver);

  if (cbw == (void *)0)
    return 0;

  uint32_t id = magic_number | u_tag << 16;

  if (!driver->init_io_msd(driver, msd_dev, cbw, blocks, start_lba, buffer,
                           magic_number, u_tag, volume, 0)) {
    driver->free_cbw(driver, cbw);
    goto write_fail;
  }
  if (driver->send_command(driver, msd_dev, cbw, &callback_cbw_data_write, 0) ==
      -1)
    goto write_fail;

  goto write_success;

write_fail:
  msd_callback callback =
      driver->callback_map->get_c(driver->callback_map, &id);
  uint8_t *target =
      driver->stored_target_map->get_c(driver->stored_target_map, &id);
  callback(target, 0);
  driver->clear_msd_map(driver, id);
  return 0;

write_success:
  return 1;
  return 1;
}

static int init_io_msd(MassStorageDriver *driver, MassStorageDev *msd_dev,
                CommandBlockWrapper *cbw, uint32_t blocks, uint64_t start_lba,
                uint8_t *buffer, uint16_t magic_number, uint8_t u_tag,
                uint8_t volume, uint8_t read) {

  uint32_t block_size = driver->get_block_size(driver, msd_dev, volume);
  uint32_t map_entry = magic_number | u_tag << 16;

  if (!driver->callback_map->contains_c(driver->callback_map, &map_entry))
    return 0;

  uint32_t *wrapper_entry =
      (uint32_t *)interface_allocateMemory(sizeof(uint32_t), 0);
  if (wrapper_entry == (void *)0)
    return 0;
  *wrapper_entry = map_entry;

  uint32_t *len_entry =
      (uint32_t *)interface_allocateMemory(sizeof(uint32_t), 0);
  if (len_entry == (void *)0) {
    interface_freeMemory(wrapper_entry, 0);
    return 0;
  }
  *len_entry = blocks * block_size;

  driver->cbw_map->put_c(driver->cbw_map, cbw, wrapper_entry);
  driver->stored_target_map->put_c(driver->stored_target_map, &map_entry,
                                   buffer);
  driver->stored_len_map->put_c(driver->stored_len_map, &map_entry, len_entry);

  uint32_t lba_low = start_lba & 0x00000000FFFFFFFF;
  uint32_t lba_high = (start_lba & 0xFFFFFFFF00000000) >> 32;

  uint32_t t_len = driver->calc_t_len(driver, msd_dev, volume, blocks);

  if (read)
    driver->build_read_command(driver, msd_dev, cbw, volume, blocks, lba_low,
                               lba_high, t_len);
  else
    driver->build_write_command(driver, msd_dev, cbw, volume, blocks, lba_low,
                                lba_high, t_len);

  return 1;
}

static uint32_t calc_t_len(MassStorageDriver *driver, MassStorageDev *msd_dev,
                    uint8_t volume, uint32_t blocks) {
  CapacityDescriptor cap_desc =
      msd_dev->mass_storage_volumes[volume].capacity_desc[0];
  uint32_t b1 = cap_desc.block_length_b1, b2 = cap_desc.block_length_b2,
           b3 = cap_desc.block_length_b3;

  uint32_t block_len = b1 | b2 << 8 | b3 << 16;

  uint32_t t_len = block_len * blocks;

  return t_len;
}

static int is_valid_volume(MassStorageDriver *driver, MassStorageDev *msd_dev,
                    uint8_t volume_input) {
  return (volume_input < msd_dev->volumes ? 1 : 0);
}

static int get_capacity_count(MassStorageDriver *driver, MassStorageDev *msd_dev,
                       uint8_t volume) {
  return msd_dev->mass_storage_volumes[volume].found_capacities;
}

static int send_inquiry(MassStorageDriver *driver, MassStorageDev *msd_dev,
                 CommandBlockWrapper *cbw, CommandStatusWrapper *csw,
                 InquiryCommandData *inquiry_data, uint8_t volume,
                 RequestSense *rs) {
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000024;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 6;

  driver->command_helper(driver, cbw->command, INQUIRY, 0, 0, 0, 0x24, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume,
                        command_len);
  if (driver->send_command(driver, msd_dev, cbw, &callback_config,
                           BULK_INITIAL_STATE) == -1)
    return -1;
  if (driver->get_data(driver, msd_dev, inquiry_data,
                       sizeof(InquiryCommandData), BULK_INITIAL_STATE,
                       &callback_config) == -1)
    return -1;
  if (driver->retrieve_status(driver, msd_dev, csw, BULK_INITIAL_STATE,
                              &callback_config) == -1) {
    driver->send_request_sense(driver, msd_dev, cbw, csw, rs, volume);
    return -1;
  }

  msd_dev->mass_storage_volumes[volume].inquiry = *inquiry_data;

  return 1;
}

// copy len bytes to target buffer : for each get function

static int get_inquiry_data(MassStorageDriver *driver, MassStorageDev *msd_dev,
                     uint8_t volume, uint8_t param, uint8_t *target,
                     uint8_t *len) {
  InquiryCommandData inquiry_data =
      msd_dev->mass_storage_volumes[volume].inquiry;

  if (param == PERI_QUALIFIER) {
    *target = (inquiry_data.byte1 & PERI_QUALIFIER) >> 5;
    *len = 1;
  } else if (param == PERI_DEVICE_TYPE) {
    *target = inquiry_data.byte1 & PERI_DEVICE_TYPE;
    *len = 1;
  } else if (param == RMB) {
    *target = (inquiry_data.byte2 & RMB) >> 7;
    *len = 1;
  } else if (param == VERSION) {
    *target = inquiry_data.byte3;
    *len = 1;
  } else if (param == NORM_ACA) {
    *target = (inquiry_data.byte4 & NORM_ACA) >> 5;
    *len = 1;
  } else if (param == HI_SUP) {
    *target = (inquiry_data.byte4 & HI_SUP) >> 4;
    *len = 1;
  } else if (param == RESPONSE_DATA_FORMAT) {
    *target = inquiry_data.byte4 & RESPONSE_DATA_FORMAT;
    *len = 1;
  } else if (param == ADDITIONAL_LEN) {
    *target = inquiry_data.byte5 & ADDITIONAL_LEN;
    *len = 1;
  } else if (param == SCCS) {
    *target = (inquiry_data.byte6 & SCCS) >> 7;
    *len = 1;
  } else if (param == ACC) {
    *target = (inquiry_data.byte6 & ACC) >> 6;
    *len = 1;
  } else if (param == TPGS) {
    *target = (inquiry_data.byte6 & TPGS) >> 4;
    *len = 1;
  } else if (param == THREE_PC) {
    *target = (inquiry_data.byte6 & THREE_PC) >> 3;
    *len = 1;
  } else if (param == PROT) {
    *target = inquiry_data.byte6 & PROT;
    *len = 1;
  } else if (param == RESV) {
    *target = (inquiry_data.byte7 & RESV) >> 7;
    *len = 1;
  } else if (param == ENC_SERV) {
    *target = (inquiry_data.byte7 & ENC_SERV) >> 6;
    *len = 1;
  } else if (param == VS) {
    *target = (inquiry_data.byte7 & VS) >> 5;
    *len = 1;
  } else if (param == MULTI_P) {
    *target = (inquiry_data.byte7 & MULTI_P) >> 4;
    *len = 1;
  } else if (param == ADDR_16) {
    *target = inquiry_data.byte7 & ADDR_16;
    *len = 1;
  } else if (param == WBUS_16) {
    *target = (inquiry_data.byte8 & WBUS_16) >> 5;
    *len = 1;
  } else if (param == SYNC_INQUIRY) {
    *target = (inquiry_data.byte8 & SYNC_INQUIRY) >> 4;
    *len = 1;
  } else if (param == CMDN_QUE) {
    *target = (inquiry_data.byte8 & CMDN_QUE) >> 1;
    *len = 1;
  }
  // copy data from kernel to target buffer, todo !
  else if (param == VENDOR_INFORMATION) {
    target = inquiry_data.vendor_information;
    *len = 8;
  } else if (param == PRODUCT_INFORMATION) {
    target = inquiry_data.product_information;
    *len = 16;
  } else if (param == PRODUCT_REVISION_LEVEL) {
    target = inquiry_data.product_revision_level;
    *len = 4;
  } else
    return -1;

  return 1;
}

static int get_capacity_descpritor(MassStorageDriver *driver, MassStorageDev *msd_dev,
                            uint8_t volume, uint8_t capacity_num, uint8_t param,
                            uint8_t *target, uint8_t *len) {
  if (capacity_num >= msd_dev->mass_storage_volumes[volume].found_capacities)
    return -1;
  CapacityDescriptor c_desc =
      msd_dev->mass_storage_volumes[volume].capacity_desc[capacity_num];

  if (param == NUMBER_OF_BLOCKS) {
    uint32_t *extended_target = (uint32_t *)target;
    *extended_target = c_desc.number_of_blocks;
    *len = 4;
  } else if (param == DESCRIPTOR_CODE) {
    *target = c_desc.code;
    *len = 1;
  } else if (param == BLOCK_LEN) {
    uint32_t *total = (uint32_t *)target;
    *total = c_desc.block_length_b1 | c_desc.block_length_b2 << 8 |
             c_desc.block_length_b3 << 16;
    *len = 4;
  } else
    return -1;

  return 1;
}

static int get_sense_data(MassStorageDriver *driver, uint8_t volume, uint8_t param,
                   uint8_t *target, uint8_t *len) {
  return -1;
}

static int get_capacity(MassStorageDriver *driver, MassStorageDev *msd_dev,
                 uint8_t volume, uint8_t param, uint8_t *target, uint8_t *len) {
  if (msd_dev->mass_storage_volumes[volume].version == READ_CAPACITY_10) {
    ReadCapacity__32_Bit rc = msd_dev->mass_storage_volumes[volume].rc_32_bit;
    if (param == LOGICAL_BLOCK_ADDRESS_32_BIT) {
      uint32_t *block_address = (uint32_t *)target;
      *block_address = rc.logical_block_address;
      *len = 4;
    } else if (param == BLOCK_SIZE_IN_BYTES_32_BIT) {
      uint32_t *block_size = (uint32_t *)target;
      *block_size = rc.block_size;
      *len = 4;
    } else
      return -1;

    return 1;
  } else if (msd_dev->mass_storage_volumes[volume].version ==
             READ_CAPACITY_16) {
    ReadCapacity__64_Bit rc = msd_dev->mass_storage_volumes[volume].rc_64_bit;
    if (param == LOGICAL_BLOCK_ADDRESS_64_BIT) {
      uint32_t *logical_block_addr = (uint32_t *)target;
      *logical_block_addr = rc.lower_logical_block_address;
      *(logical_block_addr + 1) = rc.upper_logical_block_address;

      *len = 8;
    } else if (param == BLOCK_SIZE_IN_BYTES_64_BIT) {
      uint32_t *block_size = (uint32_t *)target;
      *block_size = rc.block_size;
      *len = 4;
    } else if (param == P_TYPE) {
      *target = (rc.byte1 & P_TYPE) >> 1;
      *len = 1;
    } else if (param == PROT_EN) {
      *target = rc.byte1 & PROT_EN;
      *len = 1;
    } else if (param == P_I_EXPONENT) {
      *target = (rc.byte2 & P_I_EXPONENT) >> 4;
      *len = 1;
    } else if (param == LBS) {
      *target = rc.byte2 & LBS;
      *len = 1;
    } else if (param == LOWEST_BLOCK_ADDRESS) {
      uint16_t *l_block_addr = (uint16_t *)target;
      *l_block_addr = rc.word1;
      *len = 2;
    } else
      return -1;

    return 1;
  }

  return -1;
}

static int send_read_format_capacities(MassStorageDriver *driver,
                                MassStorageDev *msd_dev,
                                CommandBlockWrapper *cbw,
                                CommandStatusWrapper *csw,
                                CapacityListHeader *clh, uint8_t volume,
                                RequestSense *rs) {
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x000000FC;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 10;

  driver->command_helper(driver, cbw->command, READ_FORMAT_CAPACITIES, 0, 0, 0,
                         0, 0, 0, 0, 0xFC, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume,
                        command_len);
  int current_iteration = 0;
  while (current_iteration < READ_FORMAT_CAPACITIES_THRESHHOLD) {
    if (driver->send_command(driver, msd_dev, cbw, &callback_config,
                             BULK_INITIAL_STATE) == -1)
      return -1;
    if (driver->get_data(driver, msd_dev, clh, sizeof(CapacityListHeader),
                         BULK_INITIAL_STATE, &callback_config) == -1)
      return -1;
    if (driver->retrieve_status(driver, msd_dev, csw, BULK_INITIAL_STATE,
                                &callback_config))
      break;
    current_iteration++;
  }

  // not functional if it enters
  if (current_iteration == READ_FORMAT_CAPACITIES_THRESHHOLD)
    return -1;

  driver->parse_format_capacities(driver, msd_dev, clh, volume);

  return 1;
}

static void parse_format_capacities(MassStorageDriver *driver, MassStorageDev *msd_dev,
                             CapacityListHeader *clh, uint8_t volume) {
  uint8_t capacity_len = clh->capacity_len;
  uint8_t *capacity_descriptors = clh->capacity_descriptors;
  uint8_t current_capacity = 0;
  int curret_index = 0;
  while (current_capacity < capacity_len) {
    CapacityDescriptor *c_desc = (CapacityDescriptor *)capacity_descriptors;
    msd_dev->mass_storage_volumes[volume].capacity_desc[curret_index++] =
        *c_desc;
    current_capacity += sizeof(CapacityDescriptor);
  }

  msd_dev->mass_storage_volumes[volume].found_capacities = curret_index;
}

static int send_request_sense(MassStorageDriver *driver, MassStorageDev *msd_dev,
                       CommandBlockWrapper *cbw, CommandStatusWrapper *csw,
                       RequestSense *rs, uint8_t volume) {
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000012;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 6;

  driver->command_helper(driver, cbw->command, REQUEST_SENSE, 0, 0, 0, 0x12, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume,
                        command_len);

  if (driver->send_command(driver, msd_dev, cbw, &callback_config,
                           BULK_INITIAL_STATE) == -1)
    return -1;
  if (driver->get_data(driver, msd_dev, rs, sizeof(RequestSense),
                       BULK_INITIAL_STATE, &callback_config) == -1)
    return -1;
  if (driver->retrieve_status(driver, msd_dev, csw, BULK_INITIAL_STATE,
                              &callback_config) == -1)
    return -1;

  driver->parse_request_sense(driver, rs);

  return 1;
}

static void parse_request_sense(MassStorageDriver *driver, RequestSense *rs) {
  uint8_t error = rs->byte_1 & ERROR_CODE;
  uint8_t sense_key = rs->byte_2 & SENSE_KEY;
  uint8_t asc = rs->asc;
  uint8_t ascq = rs->ascq;

  uint8_t status;
  char *status_msg = "Error %u code : %u ; %s";
  if (sense_key == 0x01 && asc == 0x17 && ascq == 0x01)
    status = RECOVERED_DATA_WITH_RETRIES;
  if (sense_key == 0x01 && asc == 0x18 && ascq == 0x00)
    status = RECOVERED_DATA_WITH_ECC;
  if (sense_key == 0x02) {
    if (asc == 0x04) {
      if (ascq == 0x01)
        status = LOGICAL_DRIVE_NOT_READY_BECOMING_READY;
      if (ascq == 0x02)
        status = LOGICAL_DRIVE_NOT_READY_INIT_REQUIRED;
      if (ascq == 0x04)
        status = LOGICAL_UNIT_NOT_READY_FORMAT_IN_PROGRESS;
      if (ascq == 0xFF)
        status = LOGICAL_UNIT_NOT_READY_DEVICE_IS_BUSY;
    }
    if (asc == 0x08) {
      if (ascq == 0x00)
        status = LOGICAL_UNIT_COMMUNICATION_ERROR;
      if (ascq == 0x01)
        status = LOGICAL_UNIT_COMMUNICATION_TIMEOUT;
      if (ascq == 0x80)
        status = LOGICAL_UNIT_COMMUNICATION_OVERRUN;
    }
    if (asc == 0x3A && ascq == 0x00)
      status = MEDIUM_NOT_PRESENT;
    if (asc == 0x54 && ascq == 0x00)
      status = USB_TO_HOST_INTERFACE_FAILURE;
    if (asc == 0x80 && ascq == 0x00)
      status = INSUFFICIENT_RESOURCES;
    if (asc == 0xFF && ascq == 0xFF)
      status = UNKNOWN_ERR;
  }
  if (sense_key == 0x03) {
    if (asc == 0x02 && ascq == 0x00)
      status = NO_SEEK_COMPLETE;
    if (asc == 0x03 && ascq == 0x00)
      status = WRITE_FAULT;
    if (asc == 0x10 && ascq == 0x00)
      status = ID_CRC_ERROR;
    if (asc == 0x11 && ascq == 0x00)
      status = UNRECOVERED_READ_ERROR;
    if (asc == 0x14 && ascq == 0x00)
      status = RECORDED_ENTITY_NOT_FOUND;
    if (asc == 0x30 && ascq == 0x01)
      status = UNKNOWN_FORMAT;
  }
  if (sense_key == 0x05) {
    if (asc == 0x1A && ascq == 0x00)
      status = PARAMETER_LIST_LEN_ERR;
    if (asc == 0x20 && ascq == 0x00)
      status = INV_COMMAND_OPERATION_CODE;
    if (asc == 0x21 && ascq == 0x00)
      status = LOGICAL_BLOCK_ADDR_OUT_OF_RANGE;
    if (asc == 0x24 && ascq == 0x00)
      status = INV_FIELD_IN_COMMAND_PACKET;
    if (asc == 0x25 && ascq == 0x00)
      status = LOGICAL_UNIT_NOT_SUPPORTED;
    if (asc == 0x26 && ascq == 0x00)
      status = INV_FIELD_IN_PARAMETER_LIST;
    if (asc == 0x26 && ascq == 0x01)
      status = PARAMETER_NOT_SUPPORTED;
    if (asc == 0x26 && ascq == 0x02)
      status = PARAMETER_VALUE_INV;
  }
  if (sense_key == 0x06)
    if (asc == 0x28 && ascq == 0x00)
      status = MEDIA_CHANGED;
  if (sense_key == 0x07)
    if (asc == 0x27 && ascq == 0x00)
      status = WRITE_PROT_MEDIA;
  __DRIVER_LOGGER_INFO_CALL__(driver, status_msg, error,
    status, driver->description_sense[status]);
}

// query 32_bit first -> if 32_bit is too low make a call to 64 bit -> save flag
// in driver
static int send_read_capacity__32_bit(MassStorageDriver *driver,
                               MassStorageDev *msd_dev,
                               CommandBlockWrapper *cbw,
                               CommandStatusWrapper *csw,
                               ReadCapacity__32_Bit *rc, uint8_t volume,
                               RequestSense *rs) {
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000008;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 10;

  driver->command_helper(driver, cbw->command, READ_CAPACITY_10, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume,
                        command_len);

  if (driver->send_command(driver, msd_dev, cbw, &callback_config,
                           BULK_INITIAL_STATE) == -1)
    return -1;
  if (driver->get_data(driver, msd_dev, rc, sizeof(ReadCapacity__32_Bit),
                       BULK_INITIAL_STATE, &callback_config) == -1)
    return -1;
  if (driver->retrieve_status(driver, msd_dev, csw, BULK_INITIAL_STATE,
                              &callback_config) == -1) {
    driver->send_request_sense(driver, msd_dev, cbw, csw, rs, volume);
    return -1;
  }

  return 1;
}

static int send_read_capacity__64_bit(MassStorageDriver *driver,
                               MassStorageDev *msd_dev,
                               CommandBlockWrapper *cbw,
                               CommandStatusWrapper *csw,
                               ReadCapacity__64_Bit *rc, uint8_t volume,
                               RequestSense *rs) {
  uint32_t millis = getSystemTimeInMilli();
  uint32_t t_length = 0x00000020;
  uint8_t flags = FLAGS_IN;
  uint8_t command_len = 16;

  driver->command_helper(driver, cbw->command, READ_CAPACITY_16, 0x10, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  driver->build_command(driver, cbw, millis, t_length, flags, volume,
                        command_len);

  if (driver->send_command(driver, msd_dev, cbw, &callback_config,
                           BULK_INITIAL_STATE) == -1)
    return -1;
  if (driver->get_data(driver, msd_dev, rc, sizeof(ReadCapacity__64_Bit),
                       BULK_INITIAL_STATE, &callback_config) == -1)
    return -1;
  if (driver->retrieve_status(driver, msd_dev, csw, BULK_INITIAL_STATE,
                              &callback_config) == -1) {
    driver->send_request_sense(driver, msd_dev, cbw, csw, rs, volume);
    return -1;
  }

  return 1;
}

static void build_read_command(MassStorageDriver *driver, MassStorageDev *msd_dev,
                        CommandBlockWrapper *cbw, uint8_t volume,
                        uint32_t blocks, uint32_t lba_low, uint32_t lba_high,
                        uint32_t t_len) {
  MassStorageVolume mass_storage_volume = msd_dev->mass_storage_volumes[volume];
  uint32_t millis = getSystemTimeInMilli();
  uint8_t inner_flags = FLAGS_IN;
  // reminder big-endian when larger than 1 byte
  uint8_t block_b1 = blocks & 0xFF, block_b2 = (blocks & 0xFF00) >> 8,
          block_b3 = (blocks & 0xFF0000) >> 16,
          block_b4 = (blocks & 0xFF000000) >> 24;
  uint8_t command_len;
  if (mass_storage_volume.version == READ_CAPACITY_10) {
    command_len = 12;
    uint8_t lba_b1 = lba_low & 0xFF, lba_b2 = (lba_low & 0xFF00) >> 8,
            lba_b3 = (lba_low & 0xFF0000) >> 16,
            lba_b4 = (lba_low & 0xFF000000) >> 24;

    driver->command_helper(driver, cbw->command, READ_10, 0, lba_b4, lba_b3,
                           lba_b2, lba_b1, 0, block_b2, block_b1, 0, 0, 0, 0, 0,
                           0, 0);
  } else if (mass_storage_volume.version == READ_CAPACITY_16) {
    command_len = 16;
    uint8_t lba_b1 = lba_low & 0xFF, lba_b2 = (lba_low & 0xFF00) >> 8,
            lba_b3 = (lba_low & 0xFF0000) >> 16,
            lba_b4 = (lba_low & 0xFF000000) >> 24;
    uint8_t lba_b5 = lba_high & 0xFF, lba_b6 = (lba_high & 0xFF00) >> 8,
            lba_b7 = (lba_high & 0xFF0000) >> 16,
            lba_b8 = (lba_high & 0xFF000000) >> 24;

    driver->command_helper(driver, cbw->command, READ_16, 0, lba_b8, lba_b7,
                           lba_b6, lba_b5, lba_b4, lba_b3, lba_b2, lba_b1,
                           block_b4, block_b3, block_b2, block_b1, 0, 0);
  }

  driver->build_command(driver, cbw, millis, t_len, inner_flags, volume,
                        command_len);
}

static int send_read(MassStorageDriver *driver, MassStorageDev *msd_dev,
              CommandBlockWrapper *cbw, CommandStatusWrapper *csw,
              uint8_t *buffer, uint32_t blocks, uint8_t volume,
              uint32_t lba_low, uint32_t lba_high, callback_function callback,
              uint8_t flags, RequestSense *rs) {

  uint32_t t_len = driver->calc_t_len(driver, msd_dev, volume, blocks);

  driver->build_read_command(driver, msd_dev, cbw, volume, blocks, lba_low,
                             lba_high, t_len);

  if (driver->send_command(driver, msd_dev, cbw, callback,
                           BULK_INITIAL_STATE) == -1)
    return -1;
  if (driver->get_data(driver, msd_dev, buffer, t_len, flags,
                       &callback_config) == -1)
    return -1;
  if (driver->retrieve_status(driver, msd_dev, csw, BULK_INITIAL_STATE,
                              &callback_config) == -1) {
    driver->send_request_sense(driver, msd_dev, cbw, csw, rs, volume);
    return -1;
  }

  return 1;
}

static void build_write_command(MassStorageDriver *driver, MassStorageDev *msd_dev,
                         CommandBlockWrapper *cbw, uint8_t volume,
                         uint32_t blocks, uint32_t lba_low, uint32_t lba_high,
                         uint32_t t_len) {
  MassStorageVolume mass_storage_volume = msd_dev->mass_storage_volumes[volume];
  uint32_t millis = getSystemTimeInMilli();
  uint8_t inner_flags = FLAGS_OUT;
  // reminder big-endian when larger than 1 byte
  uint8_t block_b1 = blocks & 0xFF, block_b2 = (blocks & 0xFF00) >> 8,
          block_b3 = (blocks & 0xFF0000) >> 16,
          block_b4 = (blocks & 0xFF000000) >> 24;
  uint8_t command_len;
  if (mass_storage_volume.version == READ_CAPACITY_10) {
    command_len = 12;
    uint8_t lba_b1 = lba_low & 0xFF, lba_b2 = (lba_low & 0xFF00) >> 8,
            lba_b3 = (lba_low & 0xFF0000) >> 16,
            lba_b4 = (lba_low & 0xFF000000) >> 24;

    driver->command_helper(driver, cbw->command, WRITE_10, 0, lba_b4, lba_b3,
                           lba_b2, lba_b1, 0, block_b2, block_b1, 0, 0, 0, 0, 0,
                           0, 0);
  } else if (mass_storage_volume.version == READ_CAPACITY_16) {
    command_len = 16;
    uint8_t lba_b1 = lba_low & 0xFF, lba_b2 = (lba_low & 0xFF00) >> 8,
            lba_b3 = (lba_low & 0xFF0000) >> 16,
            lba_b4 = (lba_low & 0xFF000000) >> 24;
    uint8_t lba_b5 = lba_high & 0xFF, lba_b6 = (lba_high & 0xFF00) >> 8,
            lba_b7 = (lba_high & 0xFF0000) >> 16,
            lba_b8 = (lba_high & 0xFF000000) >> 24;

    driver->command_helper(driver, cbw->command, WRITE_16, 0, lba_b8, lba_b7,
                           lba_b6, lba_b5, lba_b4, lba_b3, lba_b2, lba_b1,
                           block_b4, block_b3, block_b2, block_b1, 0, 0);
  }

  driver->build_command(driver, cbw, millis, t_len, inner_flags, volume,
                        command_len);
}

static int send_write(MassStorageDriver *driver, MassStorageDev *msd_dev,
               CommandBlockWrapper *cbw, CommandStatusWrapper *csw,
               uint8_t *buffer, uint32_t blocks, uint8_t volume,
               uint32_t lba_low, uint32_t lba_high, callback_function callback,
               uint8_t flags, RequestSense *rs) {

  uint32_t t_len = driver->calc_t_len(driver, msd_dev, volume, blocks);

  driver->build_write_command(driver, msd_dev, cbw, volume, blocks, lba_low,
                              lba_high, t_len);

  if (driver->send_command(driver, msd_dev, cbw, callback,
                           BULK_INITIAL_STATE) == -1)
    return -1;
  if (driver->send_data(driver, msd_dev, buffer, t_len, flags,
                        &callback_config) == -1)
    return -1;
  if (driver->retrieve_status(driver, msd_dev, csw, BULK_INITIAL_STATE,
                              &callback_config) == -1) {
    driver->send_request_sense(driver, msd_dev, cbw, csw, rs, volume);
    return -1;
  }

  return 1;
}

static uint32_t get_drive_size(MassStorageDriver *driver, MassStorageDev *msd_dev,
                        uint8_t volume) {
  if (msd_dev->mass_storage_volumes[volume].found_capacities == 0)
    return 0;

  CapacityDescriptor cap_desc =
      msd_dev->mass_storage_volumes[volume].capacity_desc[0];
  uint32_t block_len = driver->get_block_size(driver, msd_dev, volume);
  uint32_t num_of_blocks = cap_desc.number_of_blocks;

  return block_len * num_of_blocks;
}

static uint32_t get_block_size(MassStorageDriver *driver, MassStorageDev *msd_dev,
                        uint8_t volume) {
  if (msd_dev->mass_storage_volumes[volume].found_capacities == 0)
    return 0;

  CapacityDescriptor cap_desc =
      msd_dev->mass_storage_volumes[volume].capacity_desc[0];
  uint32_t b1 = cap_desc.block_length_b1, b2 = cap_desc.block_length_b2,
           b3 = cap_desc.block_length_b3;

  uint32_t block_len = b1 | b2 << 8 | b3 << 16;

  return block_len;
}

static uint32_t get_block_num(MassStorageDriver *driver, MassStorageDev *msd_dev,
                       uint8_t volume) {
  if (msd_dev->mass_storage_volumes[volume].found_capacities == 0)
    return 0;

  CapacityDescriptor cap_desc =
      msd_dev->mass_storage_volumes[volume].capacity_desc[0];

  return cap_desc.number_of_blocks;
}

static int get_data(MassStorageDriver *driver, MassStorageDev *msd_dev, void *data,
             unsigned int len, uint8_t flags, callback_function callback) {
  UsbDev *dev = msd_dev->usb_dev;
  Interface *itf = msd_dev->interface;
  unsigned int endpoint = msd_dev->bulk_in_endpoint_addr;
  unsigned int pipe = usb_rcvbulkpipe(endpoint);
  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, data, len, callback, flags);
  if (flags == BULK_INITIAL_STATE) {
    if (!msd_dev->success_transfer)
      return -1;
  }
  return 1;
}

static int send_data(MassStorageDriver *driver, MassStorageDev *msd_dev, void *data,
              unsigned int len, uint8_t flags, callback_function callback) {
  UsbDev *dev = msd_dev->usb_dev;
  Interface *itf = msd_dev->interface;
  unsigned int endpoint = msd_dev->bulk_out_endpoint_addr;
  unsigned int pipe = usb_sndbulkpipe(endpoint);
  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, data, len, callback, flags);
  if (flags == BULK_INITIAL_STATE) {
    if (!msd_dev->success_transfer)
      return -1;
  }
  return 1;
}

static void build_command(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                   uint32_t tag, uint32_t transfer_length, uint8_t flags,
                   uint8_t lun, uint8_t command_len) {
  cbw->signature = CBW_SIGNATURE;
  cbw->tag = tag;
  cbw->transfer_length = transfer_length;
  cbw->flags = flags;
  cbw->lun = lun;
  cbw->command_len = command_len;
}

static int send_command(MassStorageDriver *driver, MassStorageDev *msd_dev,
                 CommandBlockWrapper *cbw, callback_function callback,
                 uint8_t flags) {
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
  UsbDev *dev = msd_dev->usb_dev;
  Interface *itf = msd_dev->interface;
  unsigned int pipe = usb_sndbulkpipe(msd_dev->bulk_out_endpoint_addr);
  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, cbw, CBW_LEN, callback, flags);
  if (flags == BULK_INITIAL_STATE)
    if (!msd_dev->success_transfer)
      return -1;
  return 1;
}

static int retrieve_status(MassStorageDriver *driver, MassStorageDev *msd_dev,
                    CommandStatusWrapper *csw, uint8_t flags,
                    callback_function callback) {
  UsbDev *dev = msd_dev->usb_dev;
  Interface *itf = msd_dev->interface;
  unsigned int pipe = usb_rcvbulkpipe(msd_dev->bulk_in_endpoint_addr);

  dev->usb_dev_bulk(dev, itf, pipe, PRIORITY_8, csw, CSW_LEN, callback, flags);

  if (flags == BULK_INITIAL_STATE) {
    if (!msd_dev->success_transfer)
      return -1;
    if (csw->status != 0 || csw->data_residue > 0)
      return -1;
  }

  return 1;
}

static uint8_t check_csw_status(MassStorageDriver *driver, CommandStatusWrapper *csw) {
  uint8_t status = csw->status;

  return status;
}

static void command_helper(MassStorageDriver *driver, uint8_t *command, uint8_t c1,
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

static int test_mass_storage_writes(MassStorageDriver *driver, MassStorageDev *msd_dev,
                             CommandBlockWrapper *cbw,
                             CommandStatusWrapper *csw, uint8_t volume,
                             RequestSense *rs) {
  uint32_t block_size = driver->get_block_size(driver, msd_dev, volume);
  uint8_t *buffer = msd_dev->buffer;
  
  __DRIVER_LOGGER_INFO_CALL__(driver, "Start writing device");
  // write 1 block starting from lba 0
  for (int i = 0; i < block_size; i++) {
    buffer[i] = 'A';
  }
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "sending %u bytes to dev, starting at lba %u",
                                block_size, 0);
  driver->send_write(driver, msd_dev, cbw, csw, buffer, 1, volume, 0, 0,
                     &callback_config, BULK_INITIAL_STATE, rs);
  // write 4 blocks starting from lba 1
  for (int i = 0; i < 4 * block_size; i++) {
    buffer[i] = 'B';
  }
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "sending %u bytes to dev, starting at lba %u",
                                block_size * 4, 1);
  driver->send_write(driver, msd_dev, cbw, csw, buffer, 4, volume, 1, 0,
                     &callback_config, BULK_INITIAL_STATE, rs);
  // write 1 block starting from lba 5
  for (int i = 0; i < block_size; i++) {
    buffer[i] = 'C';
  }
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "sending %u bytes to dev, starting at lba %u",
                                block_size, 5);
  driver->send_write(driver, msd_dev, cbw, csw, buffer, 1, volume, 5, 0,
                     &callback_config, BULK_INITIAL_STATE, rs);
  // write 20 blocks starting from lba 10
  for (int i = 0; i < 20 * block_size; i++) {
    buffer[i] = 'D';
  }
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "sending %u bytes to dev, starting at lba %u",
                                block_size * 20, 10);
  driver->send_write(driver, msd_dev, cbw, csw, buffer, 20, volume, 10, 0,
                     &callback_config, BULK_INITIAL_STATE, rs);

  // write 3 blocks starting from lba 48
  for (int i = 0; i < 3 * block_size; i++) {
    buffer[i] = 'E';
  }
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "sending %u bytes to dev, starting at lba %u",
                                block_size * 3, 48);
  driver->send_write(driver, msd_dev, cbw, csw, buffer, 3, volume, 48, 0,
                     &callback_config, BULK_INITIAL_STATE, rs);


  __DRIVER_LOGGER_INFO_CALL__(driver, "starting tests to ensure data integrity");

  // read 1 block starting from lba 0
  driver->send_read(driver, msd_dev, cbw, csw, buffer, 1, volume, 0, 0,
                    &callback_config, BULK_INITIAL_STATE, rs);
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "received %u bytes from dev, started at lba %u",
                                block_size, 0);
  int test_case = 1;
  int test_failed = 0;
  int test_one_failed = 0;
  for (int i = 0; i < block_size; i++) {
    if (buffer[i] != 'A') {
      test_failed = 1;
      test_one_failed = 1;
      __DRIVER_LOGGER_INFO_CALL__(driver, "test %d failed",
                                    test_case);
      break;
    }
  }
  if (!test_failed)
    __DRIVER_LOGGER_INFO_CALL__(driver, "test %d passed",
                                  test_case);
  test_case++;
  test_failed = 0;

  // read 4 blocks starting from lba 1
  driver->send_read(driver, msd_dev, cbw, csw, buffer, 4, volume, 1, 0,
                    &callback_config, BULK_INITIAL_STATE, rs);
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "received %u bytes from dev, started at lba %u",
                                block_size * 4, 1);
  for (int i = 0; i < 4 * block_size; i++) {
    if (buffer[i] != 'B') {
      test_failed = 1;
      test_one_failed = 1;
      __DRIVER_LOGGER_INFO_CALL__(driver, "test %d failed",
                                    test_case);
      break;
    }
  }
  if (!test_failed)
    __DRIVER_LOGGER_INFO_CALL__(driver, "test %d passed",
                                  test_case);
  test_case++;
  test_failed = 0;
  // read 1 block starting from lba 5
  driver->send_read(driver, msd_dev, cbw, csw, buffer, 1, volume, 5, 0,
                    &callback_config, BULK_INITIAL_STATE, rs);
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "received %u bytes from dev, started at lba %u",
                                block_size, 5);

  for (int i = 0; i < block_size; i++) {
    if (buffer[i] != 'C') {
      test_failed = 1;
      test_one_failed = 1;
      __DRIVER_LOGGER_INFO_CALL__(driver, "test %d failed",
                                    test_case);
      break;
    }
  }
  if (!test_failed)
    __DRIVER_LOGGER_INFO_CALL__(driver, "test %d passed",
                                  test_case);
  test_case++;
  test_failed = 0;

  // read 20 blocks starting from lba 10
  driver->send_read(driver, msd_dev, cbw, csw, buffer, 20, volume, 10, 0,
                    &callback_config, BULK_INITIAL_STATE, rs);
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "received %u bytes from dev, started at lba %u",
                                block_size * 20, 10);

  for (int i = 0; i < 20 * block_size; i++) {
    if (buffer[i] != 'D') {
      test_failed = 1;
      test_one_failed = 1;
      __DRIVER_LOGGER_INFO_CALL__(driver, "test %d failed",
                                    test_case);
      break;
    }
  }
  if (!test_failed)
    __DRIVER_LOGGER_INFO_CALL__(driver, "test %d passed",
                                  test_case);
  test_case++;
  test_failed = 0;

  // read 3 blocks starting from lba 48
  driver->send_read(driver, msd_dev, cbw, csw, buffer, 3, volume, 48, 0,
                    &callback_config, BULK_INITIAL_STATE, rs);
  __DRIVER_LOGGER_INFO_CALL__(driver,
                                "received %u bytes from dev, started at lba %u",
                                block_size * 3, 48);

  for (int i = 0; i < 3 * block_size; i++) {
    if (buffer[i] != 'E') {
      test_failed = 1;
      test_one_failed = 1;
      __DRIVER_LOGGER_INFO_CALL__(driver, "test %d failed",
                                    test_case);
      break;
    }
  }
  if (!test_failed)
    __DRIVER_LOGGER_INFO_CALL__(driver, "test %d passed",
                                  test_case);

  if (!test_one_failed)
    __DRIVER_LOGGER_INFO_CALL__(driver,
        "all tests passed - device is working properly");
  else
    __DRIVER_LOGGER_INFO_CALL__(driver,
        "not all tests passed - device is not working properly");
  return 1;
}