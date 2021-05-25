//
// Created by hannes on 15.05.21.
//

#include "IP4Netmask.h"

IP4Netmask::IP4Netmask(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) {
    netmask[0] = first;
    netmask[1] = second;
    netmask[2] = third;
    netmask[3] = fourth;
}
