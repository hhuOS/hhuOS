#ifndef UHCI_DATA__INCLUDE
#define UHCI_DATA__INCLUDE

#include "stdint.h"

struct QH {
  volatile uint32_t pyhsicalQHLP;
  volatile uint32_t pyhsicalQHEP;
  volatile uint32_t parent;
  volatile uint32_t flags;
} __attribute__((packed));

struct TD {
  volatile uint32_t pyhsicalLinkPointer;
  volatile uint32_t control_x_status;
  volatile uint32_t token;
  volatile uint32_t bufferPointer;
} __attribute__((packed));

typedef struct QH QH;
typedef struct TD TD;

#define SKELETON_SIZE 11

struct MQH{
    uint8_t size;
    uint16_t qh[SKELETON_SIZE];
};

#define FRAME_SCHEDULE (struct MQH){SKELETON_SIZE,{1,2,4,8,16,32,64,128,256,512,1024}}

enum PID { // 7:4 compl , 3:0 val
  OUT = 0xE1,
  IN = 0x69,
  SOF_PID = 0xA5,
  SETUP = 0x2D,
  DATA0 = 0xC3,
  DATA1 = 0x4B,
  DATA2 = 0x87,
  MDATA = 0x0F,
  ACK = 0xD2,
  NAK = 0x5A,
  STALL = 0x1E,
  NYET = 0x96,
  PRE = 0x3C,
  SPLIT = 0x78,
  PING = 0xB4,
};

enum QH_HEADS {
  QH_1024 = 0x00,
  QH_512 = 0x01,
  QH_256 = 0x02,
  QH_128 = 0x03,
  QH_64 = 0x04,
  QH_32 = 0x05,
  QH_16 = 0x06,
  QH_8 = 0x07,
  QH_4 = 0x08,
  QH_2 = 0x09,
  QH_1 = 0x0A,
  QH_CTL = 0x0B,
  QH_BULK = 0x0C
};

struct UsbPacket {
  struct TD *internalTD;
};

struct UsbTransaction {
  struct UsbPacket *entry_packet;
  struct UsbTransaction *next;
  const char *transaction_type;
};

struct UsbTransfer {
  struct UsbTransaction *entry_transaction;
  const char *transfer_type;
  int transaction_count;
};

typedef struct UsbPacket UsbPacket;
typedef struct UsbTransaction UsbTransaction;
typedef struct UsbTransfer UsbTransfer;

// just a container
struct TokenValues {
  int16_t max_len;
  uint8_t toggle;
  uint8_t endpoint;
  uint8_t address;
  uint8_t packet_type;
};

typedef struct TokenValues TokenValues;
typedef uint8_t *(*setup_handler)(void);

#endif