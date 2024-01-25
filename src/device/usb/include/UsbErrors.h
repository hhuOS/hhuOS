/******

Contains all Errors which can occur while transfering or init transfer

*****/

#ifndef USB_ERRORS_INCLUDE
#define USB_ERRORS_INCLUDE

#include "stdint.h"

extern const uint32_t E_ENDPOINT_INV;

extern const uint32_t E_INTERFACE_INV;

extern const uint32_t E_INTERFACE_IN_USE;

extern const uint32_t E_INVALID_SETUP;

extern const uint32_t E_TRANSFER;

extern const uint32_t E_INVALID_INTERVAL;

extern const uint32_t E_NOT_SUPPORTED_TRANSFER_TYPE;

extern const uint32_t E_PRIORITY_NOT_SUPPORTED;

extern const uint32_t E_DEVICE_NOT_SUPPORTED;

extern const uint32_t E_INTERFACE_NOT_SUPPORTED;

extern const uint32_t RETRANSMITTING_AFTER_ERROR;

extern const uint32_t RETRANSMITTING_AFTER_SUCCESS;

extern const uint32_t S_TRANSFER;

// extern const uint32_t E_SPD_STATUS;

extern const uint32_t E_STALLED;

extern const uint32_t E_DATA_BUFFER;

extern const uint32_t E_BABBLE_DETECTED;

extern const uint32_t E_NAK_RECEIVED;

extern const uint32_t E_CRC;

extern const uint32_t E_BITSTUFF;

/*********************************************/

#endif