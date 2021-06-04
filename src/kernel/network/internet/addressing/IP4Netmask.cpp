//
// Created by hannes on 15.05.21.
//

#include "IP4Netmask.h"

IP4Netmask::IP4Netmask(uint8_t bitCount) {
    uint8_t maxBitCount = IP4ADDRESS_LENGTH * 8;
    if (bitCount > maxBitCount) {
        bitCount = maxBitCount;
    }
    this->bitCount = bitCount;

    uint8_t fullByteCount = bitCount / 8;
    for (uint8_t i = 0; i <= fullByteCount; i++) {
        this->netmask[i] = -1;
    }
    this->netmask[fullByteCount] = this->netmask[fullByteCount] << (8 - (bitCount % 8));
}

IP4Address *IP4Netmask::extractNetPart(IP4Address *ip4Address) {
    return ip4Address->calculateAND(this->netmask);
}

uint8_t IP4Netmask::getBitCount() const {
    return this->bitCount;
}

String IP4Netmask::asString() {
    return String::format("%d.%d.%d.%d /%d", netmask[0], netmask[1], netmask[2], netmask[3], bitCount);
}
