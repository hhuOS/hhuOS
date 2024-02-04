#ifndef MASS_STORAGE_DRIVER__INCLUDE
#define MASS_STORAGE_DRIVER__INCLUDE

#include "../UsbDriver.h"
#include "stdint.h"
#include "../../events/event/Event.h"
#include "../../events/event/storage/MassStorageEvent.h"
#include "CommandInterface.h"

#define MAX_TRANSFER_BYTES 0x00008000
#define ALLOWED_CAPACITES 31
#define READ_FORMAT_CAPACITIES_THRESHHOLD 5 // atleast 5 iterations 
#define MAX_VOLUMES 16
#define TEST_ON

struct MassStorageDev{
    UsbDev* usb_dev;
    unsigned int bulk_out_endpoint_addr;
    unsigned int bulk_in_endpoint_addr;
    void* buffer; // just store MAX_TRANSFER_BYTES max not complete drive
    unsigned int buffer_size;
    uint8_t priority;
    Interface* interface;
    UsbDriver* usb_driver;

    void (*callback)(UsbDev* dev, uint32_t status, void* data);
    void (*callback_cbw)(UsbDev* dev, uint32_t status, void* cbw);
    void (*callback_csw)(UsbDev* dev, uint32_t status, void* csw);
};

struct MassStorageVolume{
    void (*new_storage_volume)(struct MassStorageVolume* volume, uint8_t volume_num);
    uint8_t version;
    uint8_t volume_number;

    InquiryCommandData inquiry;

    CapacityDescriptor capacity_desc[ALLOWED_CAPACITES];
    unsigned int found_capacities;

    uint32_t volume_size;
    uint32_t block_size;
    uint32_t block_num;

    ReadCapacity__32_Bit rc_32_bit; // filter via version
    ReadCapacity__64_Bit rc_64_bit;
};

void new_storage_volume(struct MassStorageVolume* volume, uint8_t volume_num);

struct MassStorageDriver{
    struct UsbDriver super;
    struct MassStorageDev dev;

    void (*new_mass_storage_driver)(struct MassStorageDriver* driver, char* name, UsbDevice_ID* entry);

    int (*configure_device)(struct MassStorageDriver *driver);
    int (*read_command)(struct MassStorageDriver* driver, CommandCode cc, void* data);
    int (*send_inquiry)(struct MassStorageDriver *driver, CommandBlockWrapper *cbw,
                    CommandStatusWrapper *csw, InquiryCommandData *inquiry_data,
                    uint8_t volume, RequestSense* rs);
    int (*send_read_format_capacities)(struct MassStorageDriver *driver, CommandBlockWrapper *cbw,
                                    CommandStatusWrapper* csw, CapacityListHeader* clh,
                                    uint8_t volume, RequestSense* rs);
    void (*parse_format_capacities)(struct MassStorageDriver* driver, CapacityListHeader* clh, uint8_t volume);
    int (*get_data)(struct MassStorageDriver *driver, void *data, unsigned int len, uint8_t flags);                       
    int (*send_data)(struct MassStorageDriver *driver, void *data, unsigned int len, uint8_t flags);
    void (*build_command)(struct MassStorageDriver *driver, CommandBlockWrapper *cbw,
                    uint32_t tag, uint32_t transfer_length, uint8_t flags,
                    uint8_t lun, uint8_t command_len);
    int (*send_command)(struct MassStorageDriver *driver, CommandBlockWrapper *cbw,
                    callback_function callback);  
    int (*retrieve_status)(struct MassStorageDriver *driver, CommandStatusWrapper *csw);
    uint8_t (*check_csw_status)(struct MassStorageDriver *driver, CommandStatusWrapper *csw);
    void (*command_helper)(struct MassStorageDriver *driver, uint8_t *command, uint8_t c1,
                        uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6,
                        uint8_t c7, uint8_t c8, uint8_t c9, uint8_t c10,
                        uint8_t c11, uint8_t c12, uint8_t c13, uint8_t c14,
                        uint8_t c15, uint8_t c16);
    int (*send_request_sense)(struct MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                       RequestSense* rs, uint8_t volume);
    int (*send_read_capacity__32_bit)(struct MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                                ReadCapacity__32_Bit* rc, uint8_t volume, RequestSense* rs);
    int (*send_read_capacity__64_bit)(struct MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                                ReadCapacity__64_Bit* rc, uint8_t volume, RequestSense* rs);                                                                          
    int (*send_read)(struct MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                uint8_t* buffer, uint32_t blocks, uint8_t volume, uint32_t lba_low, uint32_t lba_high,
                callback_function callback, uint8_t flags, RequestSense* rs);
    int (*send_write)(struct MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                uint8_t* buffer, uint32_t blocks, uint8_t volume, uint32_t lba_low, uint32_t lba_high,
                callback_function callback, uint8_t flags, RequestSense* rs);              
    uint32_t (*get_drive_size)(struct MassStorageDriver* driver, uint8_t volume);
    uint32_t (*get_block_size)(struct MassStorageDriver* driver, uint8_t volume);
    uint32_t (*get_block_num)(struct MassStorageDriver* driver, uint8_t volume);
    int (*test_mass_storage_writes)(struct MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw, uint8_t volume, RequestSense* rs);
    uint8_t success_transfer;

    struct MassStorageVolume* mass_storage_volumes;
    uint8_t volumes; // 0 = 1, 1 = 2 ... 15 = 16 are assuming consecutively
};

typedef struct MassStorageDriver MassStorageDriver;
typedef struct MassStorageDev MassStorageDev;
typedef struct MassStorageVolume MassStorageVolume;

void new_mass_storage_driver(MassStorageDriver* driver, char* name, UsbDevice_ID* entry);
int16_t probe_mass_storage(UsbDev *dev, Interface *interface);
void callback_mass_storage(UsbDev *dev, uint32_t status, void *data);
void disconnect_mass_storage(UsbDev* dev, Interface* interface);
int configure_device(MassStorageDriver *driver);
int read_command(MassStorageDriver* driver, CommandCode cc, void* data);
int send_inquiry(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                 CommandStatusWrapper *csw, InquiryCommandData *inquiry_data,
                 uint8_t volume, RequestSense* rs);
int send_read_format_capacities(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                                CommandStatusWrapper* csw, CapacityListHeader* clh,
                                uint8_t volume, RequestSense* rs);
void parse_format_capacities(MassStorageDriver* driver, CapacityListHeader* clh, uint8_t volume);
int get_data(MassStorageDriver *driver, void *data, unsigned int len, uint8_t flags);                       
int send_data(MassStorageDriver *driver, void *data, unsigned int len, uint8_t flags);
void build_command(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                   uint32_t tag, uint32_t transfer_length, uint8_t flags,
                   uint8_t lun, uint8_t command_len);
int send_command(MassStorageDriver *driver, CommandBlockWrapper *cbw,
                 callback_function callback);  
int retrieve_status(MassStorageDriver *driver, CommandStatusWrapper *csw);
uint8_t check_csw_status(MassStorageDriver *driver, CommandStatusWrapper *csw);
void command_helper(MassStorageDriver *driver, uint8_t *command, uint8_t c1,
                    uint8_t c2, uint8_t c3, uint8_t c4, uint8_t c5, uint8_t c6,
                    uint8_t c7, uint8_t c8, uint8_t c9, uint8_t c10,
                    uint8_t c11, uint8_t c12, uint8_t c13, uint8_t c14,
                    uint8_t c15, uint8_t c16);
int send_request_sense(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                       RequestSense* rs, uint8_t volume);
int send_read_capacity__32_bit(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                               ReadCapacity__32_Bit* rc, uint8_t volume, RequestSense* rs);
int send_read_capacity__64_bit(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
                               ReadCapacity__64_Bit* rc, uint8_t volume, RequestSense* rs);                                                                          
int send_read(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
              uint8_t* buffer, uint32_t blocks, uint8_t volume, uint32_t lba_low, uint32_t lba_high,
              callback_function callback, uint8_t flags, RequestSense* rs);
int send_write(MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw,
               uint8_t* buffer, uint32_t blocks, uint8_t volume, uint32_t lba_low, uint32_t lba_high,
               callback_function callback, uint8_t flags, RequestSense* rs);              
uint32_t get_drive_size(MassStorageDriver* driver, uint8_t volume);
uint32_t get_block_size(MassStorageDriver* driver, uint8_t volume);
uint32_t get_block_num(MassStorageDriver* driver, uint8_t volume);
int test_mass_storage_writes(struct MassStorageDriver* driver, CommandBlockWrapper* cbw, CommandStatusWrapper* csw, uint8_t volume, RequestSense* rs);

#endif