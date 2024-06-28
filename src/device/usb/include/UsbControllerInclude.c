#include "UsbControllerInclude.h"
#include "stdint.h"

const uint16_t interval_1024_ms = 1024;
const uint16_t interval_512_ms  = 512;
const uint16_t interval_256_ms  = 256;
const uint16_t interval_128_ms  = 128;
const uint16_t interval_64_ms   = 64;
const uint16_t interval_32_ms   = 32;
const uint16_t interval_16_ms   = 16;
const uint16_t interval_8_ms    = 8;
const uint16_t interval_4_ms    = 4;
const uint16_t interval_2_ms    = 2;
const uint16_t interval_1_ms    = 1;

const uint8_t PRIORITY_1 = 0;
const uint8_t PRIORITY_2 = 1;
const uint8_t PRIORITY_3 = 2;
const uint8_t PRIORITY_4 = 3;
const uint8_t PRIORITY_5 = 4;
const uint8_t PRIORITY_6 = 5;
const uint8_t PRIORITY_7 = 6;
const uint8_t PRIORITY_8 = 7;

const uint16_t MOUSE_LISTENER = 0x0000;
const uint16_t KEY_BOARD_LISTENER = 0x0001;
const uint16_t AUDIO_LISTENER = 0x0002;

const uint16_t GET_DESCRIPTOR_SETUP = 0x0001;
const uint16_t GET_CONFIGURATION_SETUP = 0x0002;
const uint16_t GET_INTERFACE_SETUP = 0x0003;
const uint16_t SET_INTERFACE_SETUP = 0x0004;
const uint16_t GET_STATUS_SETUP = 0x0005;

const int DRIVER_REMOVED = 0x1;
const int DRIVER_NOT_FOUND = 0x2;
const int DRIVER_LINKED = 0x4;
const int DRIVER_NOT_LINKED = 0x8;