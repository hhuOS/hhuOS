//
// Created by hannes on 15.05.21.
//

#include "IP4Netmask.h"

IP4Netmask::IP4Netmask(uint8_t bitCount) {
    this->bitCount = bitCount;
    if (this->bitCount > 32) {
        this->bitCount = 32;
    }
    //Set all bits to '1'
    this->netmask = UINT32_MAX;
    //Set right [bitCount] bits to '0'
    this->netmask = this->netmask << (32 - bitCount);
}

IP4Address *IP4Netmask::extractNetPart(IP4Address *ip4Address) {
    uint32_t result = ip4Address->asInt() & this->netmask;
    return new IP4Address(result);
}

uint8_t IP4Netmask::getBitCount() const {
    return this->bitCount;
}
