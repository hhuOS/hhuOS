#ifndef UHCI_FLAGS_INCLUDE
#define UHCI_FLAGS_INCLUDE

#include "stdint.h"

#define BAR 0x8
#define OFFSET 0x20

#define SOF_FLAG 0x40          // signals start of frame
#define TOTAL_FRAMES 1024 // 4KB aligned

#define PRIORITY_QH_1 0x00 << 1
#define PRIORITY_QH_2 0x01 << 1
#define PRIORITY_QH_3 0x02 << 1
#define PRIORITY_QH_4 0x03 << 1
#define PRIORITY_QH_5 0x04 << 1
#define PRIORITY_QH_6 0x05 << 1
#define PRIORITY_QH_7 0x06 << 1
#define PRIORITY_QH_8 0x07 << 1
#define PRIORITY_QH_MASK 0x0E

#define QH_FLAG_END 0x01
#define QH_FLAG_IN 0x00
#define QH_FLAG_END_MASK 0x01

#define QH_FLAG_IS_MQH 0x01 << 4
#define QH_FLAG_IS_QH 0x00 << 4
#define QH_FLAG_IS_MASK 0x10

#define QH_FLAG_TYPE_INTERRUPT 0x00 << 5
#define QH_FLAG_TYPE_ISO 0x01 << 5
#define QH_FLAG_TYPE_BULK 0x02 << 5
#define QH_FLAG_TYPE_CONTROL 0x03 << 5
#define QH_FLAG_TYPE_MASK 0x60

#define QH_FLAG_RESERVED 0x80
#define QH_ADDRESS_MASK 0xFFFFFFF0

#define QH_FLAG_DEVICE_COUNT_MASK 0xFFFFFF00
#define QH_FLAG_DEVICE_COUNT_SHIFT 0x08

#define QH_TD_SELECT 0x02
#define QH_SELECT 0x02
#define TD_SELECT 0x00
#define QH_TERMINATE 0x01
#define QH_LP_MASK 0xFFFFFFF0
#define DEPTH_BREADTH_SELECT 0x04

#define SPD 0x1D
#define C_ERR 0x1B
#define LS 0x1A
#define IOS 0x19
#define IOC 0x18
#define ACTIVE 0x17
#define NAK_RECV 0x13
#define STALLED 0x16
#define DATA_BUFFER_ERROR 0x15
#define BABBLE_DETECTED 0X14
#define CRC 0x12
#define BITSTUFF_ERROR 0x11
#define ACTUAL_LENGTH_MASK 0x7FF

#define TD_MAX_LENGTH 0x15
#define TD_DATA_TOGGLE 0x13
#define TD_ENDPOINT 0x0F
#define TD_DEVICE_ADDRESS 0x08

#define TD_MAX_LENGTH_MASK 0xFFE00000

#define RETRANSMISSON_TRIES 3

#define DIRECTION_IN_PIPE DIRECTION_IN
#define DIRECTION_OUT_PIPE DIRECTION_OUT

#define BULK_PIPE TRANSFER_TYPE_BULK << 5
#define INTERRUPT_PIPE TRANSFER_TYPE_INTERRUPT << 5
#define ISO_PIPE TRANSFER_TYPE_ISO << 5
#define CONTROL_PIPE TRANSFER_TYPE_CONTROL << 5
#define CONTROL_PIPE_MASK 0x60

#endif