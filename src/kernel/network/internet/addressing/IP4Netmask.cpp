//
// Created by hannes on 15.05.21.
//

#include "IP4Netmask.h"

IP4Netmask::IP4Netmask(uint8_t bitCount) {
    this->bitCount = bitCount;
    if (this->bitCount > 32) {
        this->bitCount = 32;
    }
    //Set all bits to '1' (works because netmask is unsigned)
    uint32_t tempMask = -1;
    //Set right [bitCount] bits to '0'
    tempMask = tempMask << (32 - bitCount);

    //Swap byte order so that bytes in IP4Netmask have the same order as bytes in IP4Address
    //Also known as Endianess, which may differ between Big Endian and Little Endian
    //TODO: Use htonl() or ntohl() from netinet/in.h
    auto tempParts = (uint8_t *) &tempMask;
    auto *maskParts = (uint8_t *) &netmask;

    for(uint8_t i=0;i<4;i++){
        maskParts[i]=tempParts[3-i];
    }
}

IP4Address *IP4Netmask::extractNetPart(IP4Address *ip4Address) {
    return new IP4Address(ip4Address->asInt() & this->netmask);
}

uint8_t IP4Netmask::getBitCount() const {
    return this->bitCount;
}

String IP4Netmask::asString() {
    auto *parts = (uint8_t *) &netmask;
    return String::format("%d.%d.%d.%d /%d", parts[0], parts[1], parts[2], parts[3], bitCount);
}
