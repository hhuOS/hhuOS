//
// Created by hannes on 14.05.21.
//

#include "IP4Address.h"

IP4Address::IP4Address(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) {
    address[0] = first;
    address[1] = second;
    address[2] = third;
    address[3] = fourth;
}