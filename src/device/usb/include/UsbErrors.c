#include "UsbErrors.h"
#include "stdint.h"

const uint32_t E_ENDPOINT_INV = 0x01;

const uint32_t E_INTERFACE_INV = 0x02;

const uint32_t E_INTERFACE_IN_USE = 0x04;

const uint32_t E_INVALID_SETUP = 0x08;

const uint32_t E_TRANSFER = 0x10;

const uint32_t E_INVALID_INTERVAL = 0x20;

const uint32_t E_NOT_SUPPORTED_TRANSFER_TYPE = 0x40;

const uint32_t E_PRIORITY_NOT_SUPPORTED = 0x80;

const uint32_t E_DEVICE_NOT_SUPPORTED = 0x100;

const uint32_t E_INTERFACE_NOT_SUPPORTED = 0x200;

const uint32_t RETRANSMITTING_AFTER_ERROR = 0x400;

const uint32_t RETRANSMITTING_AFTER_SUCCESS = 0x800;

const uint32_t S_TRANSFER = 0x1000;

//const uint32_t E_SPD_STATUS = 0x2000;

const uint32_t E_STALLED = 0x4000;

const uint32_t E_DATA_BUFFER = 0x8000;

const uint32_t E_BABBLE_DETECTED = 0x10000;

const uint32_t E_NAK_RECEIVED = 0x20000;

const uint32_t E_CRC = 0x40000;

const uint32_t E_BITSTUFF = 0x80000;