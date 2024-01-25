#ifndef UsbControllerInclude__include
#define UsbControllerInclude__include

#include "stdint.h"

extern const uint16_t interval_1024_ms;
extern const uint16_t interval_512_ms;
extern const uint16_t interval_256_ms;
extern const uint16_t interval_128_ms;
extern const uint16_t interval_64_ms;
extern const uint16_t interval_32_ms;
extern const uint16_t interval_16_ms;
extern const uint16_t interval_8_ms;
extern const uint16_t interval_4_ms;
extern const uint16_t interval_2_ms;
extern const uint16_t interval_1_ms;

extern const uint8_t PRIORITY_1;
extern const uint8_t PRIORITY_2;
extern const uint8_t PRIORITY_3;
extern const uint8_t PRIORITY_4;
extern const uint8_t PRIORITY_5;
extern const uint8_t PRIORITY_6;
extern const uint8_t PRIORITY_7;
extern const uint8_t PRIORITY_8;
 
extern const uint16_t MOUSE_LISTENER;
extern const uint16_t KEY_BOARD_LISTENER;
 
extern const uint16_t GET_DESCRIPTOR_SETUP;
extern const uint16_t GET_CONFIGURATION_SETUP;
extern const uint16_t GET_INTERFACE_SETUP;
extern const uint16_t SET_INTERFACE_SETUP;
extern const uint16_t GET_STATUS_SETUP;

#endif