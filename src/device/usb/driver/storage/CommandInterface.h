#ifndef COMMAND_INTERFACE__INCLUDE
#define COMMAND_INTERFACE__INCLUDE

#include "stdint.h"

#define CBW_SIGNATURE 0x43425355
#define CSW_SIGNATURE 0x53425355

#define FLAGS_IN 0x80
#define FLAGS_OUT 0x00

#define CBW_LEN 31
#define CSW_LEN 13
#define COMMAND_LEN 16

struct CommandBlockWrapper {
  uint32_t signature;
  uint32_t tag;
  uint32_t transfer_length;
  uint8_t flags;
  uint8_t lun;
  uint8_t command_len;
  uint8_t command[COMMAND_LEN]; // command data block consist of 16 bytes
};

struct CommandStatusWrapper {
  uint32_t signature;
  uint32_t tag;
  uint32_t data_residue; // diff in data transfered -> sucess = 0
  uint8_t status;
};

enum CommandCode {
  REQUEST_SENSE = 0x03,
  INQUIRY = 0x12,
  READ_FORMAT_CAPACITIES = 0x23,
  READ_CAPACITY_10 = 0x25,
  READ_10 = 0x28,
  WRITE_10 = 0x2A,
  READ_16 = 0x88,
  WRITE_16 = 0x8A,
  READ_CAPACITY_16 = 0x9E,
  REPORT_LUNS = 0xA0,
  READ_12 = 0xA8,
  WRITE_12 = 0xAA
};

enum InquiryData{
  PERI_QUALIFIER,
  PERI_DEVICE_TYPE,
  RMB,
  VERSION,
  NORM_ACA,
  HI_SUP,
  RESPONSE_DATA_FORMAT,
  ADDITIONAL_LEN,
  SCCS,
  ACC,
  TPGS,
  THREE_PC,
  PROT,
  RESV,
  ENC_SERV,
  VS,
  MULTI_P,
  ADDR_16,
  WBUS_16,
  SYNC_INQUIRY,
  CMDN_QUE,
  VENDOR_INFORMATION,
  PRODUCT_INFORMATION,
  PRODUCT_REVISION_LEVEL
};

enum CapacityDescriptorData{
  NUMBER_OF_BLOCKS,
  DESCRIPTOR_CODE,
  BLOCK_LEN
};

enum RequestSenseData{
  VALID,
  ERROR_CODE = 0x7F,
  FILE_MARK = 0x80,
  EOM = 0x40,
  ILI = 0x20,
  SENSE_RESV = 0x10,
  SENSE_KEY = 0x0F,
  SENSE_INFORMATION,
  ADDITIONAL_SENSE_LEN,
  ADDITIONAL_SENSE_CODE,
  ADDITIONAL_SENSE_CODE_QUALIFIER,
  FIELD_REPLACEABLE_UNIT_CODE,
  SENSE_KEY_SPECIFIC
};

#define SENSE_STATUS_LEN 29

enum SenseStatus{
  RECOVERED_DATA_WITH_RETRIES = 0,
  RECOVERED_DATA_WITH_ECC,
  LOGICAL_DRIVE_NOT_READY_BECOMING_READY,
  LOGICAL_DRIVE_NOT_READY_INIT_REQUIRED,
  LOGICAL_UNIT_NOT_READY_FORMAT_IN_PROGRESS,
  LOGICAL_UNIT_NOT_READY_DEVICE_IS_BUSY,
  LOGICAL_UNIT_COMMUNICATION_ERROR,
  LOGICAL_UNIT_COMMUNICATION_TIMEOUT,
  LOGICAL_UNIT_COMMUNICATION_OVERRUN,
  MEDIUM_NOT_PRESENT,
  USB_TO_HOST_INTERFACE_FAILURE,
  INSUFFICIENT_RESOURCES,
  UNKNOWN_ERR,
  NO_SEEK_COMPLETE,
  WRITE_FAULT,
  ID_CRC_ERROR,
  UNRECOVERED_READ_ERROR,
  RECORDED_ENTITY_NOT_FOUND,
  UNKNOWN_FORMAT,
  PARAMETER_LIST_LEN_ERR,
  INV_COMMAND_OPERATION_CODE,
  LOGICAL_BLOCK_ADDR_OUT_OF_RANGE,
  INV_FIELD_IN_COMMAND_PACKET,
  LOGICAL_UNIT_NOT_SUPPORTED,
  INV_FIELD_IN_PARAMETER_LIST,
  PARAMETER_NOT_SUPPORTED,
  PARAMETER_VALUE_INV,
  MEDIA_CHANGED,
  WRITE_PROT_MEDIA
};

enum ReadCapacityData__32_BIT{
  LOGICAL_BLOCK_ADDRESS_32_BIT,
  BLOCK_SIZE_IN_BYTES_32_BIT
};

enum ReadCapacityData__64_BIT{
  LOGICAL_BLOCK_ADDRESS_64_BIT,
  BLOCK_SIZE_IN_BYTES_64_BIT,
  P_TYPE,
  PROT_EN,
  P_I_EXPONENT,
  LBS,
  LOWEST_BLOCK_ADDRESS,
};

struct InquiryCommandData {
  uint8_t byte1;
  uint8_t byte2;
  uint8_t byte3;
  uint8_t byte4;
  uint8_t byte5;
  uint8_t byte6;
  uint8_t byte7;
  uint8_t byte8;
  uint8_t vendor_information[8];
  uint8_t product_information[16];
  uint8_t product_revision_level[4];
} __attribute__((packed));

struct CapacityListHeader {
  uint8_t reserved1;
  uint8_t reserved2;
  uint8_t reserved3;
  uint8_t capacity_len;
  uint8_t capacity_descriptors[252];
} __attribute__((packed));

struct CapacityDescriptor {
  uint32_t number_of_blocks;
  uint8_t code;
  uint8_t block_length_b1;
  uint8_t block_length_b2;
  uint8_t block_length_b3;
} __attribute__((packed));

struct RequestSense{ // we will fix this at 18 bytes
  uint8_t byte_1;
  uint8_t reserved_1;
  uint8_t byte_2;
  uint32_t information;
  uint8_t additional_sense_len;
  uint32_t reserved_2;
  uint8_t asc;
  uint8_t ascq;
  uint8_t field_replaceable_unit_code;
  uint8_t sense_key_specific_1;
  uint8_t sense_key_specific_2;
  uint8_t sense_key_specific_3;
} __attribute__((packed));

struct ReadCapacity__32_Bit{
  uint32_t logical_block_address;
  uint32_t block_size;
} __attribute__((packed));

struct ReadCapacity__64_Bit{
  uint32_t lower_logical_block_address;
  uint32_t upper_logical_block_address;
  uint32_t block_size;
  uint8_t byte1;
  uint8_t byte2;
  uint16_t word1;
  uint8_t reserved[16];
} __attribute__((packed));

#define MAXIMUM_CAPACITY_1 0b01
#define CURRENT_CAPACITY 0b10
#define MAXIMUM_CAPACITY_2 0b11

typedef struct InquiryCommandData InquiryCommandData;
typedef struct CapacityListHeader CapacityListHeader;
typedef struct CapacityDescriptor CapacityDescriptor;
typedef struct RequestSense RequestSense;
typedef struct ReadCapacity__32_Bit ReadCapacity__32_Bit;
typedef struct ReadCapacity__64_Bit ReadCapacity__64_Bit;
typedef struct CommandBlockWrapper CommandBlockWrapper;
typedef struct CommandStatusWrapper CommandStatusWrapper;
typedef enum CommandCode CommandCode;

#endif