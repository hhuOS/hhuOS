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
    calculateBitmask(this->netmask, bitCount);
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

void IP4Netmask::calculateBitmask(uint8_t *target, uint8_t oneBitNumber) {
    auto fullByteCount = (uint8_t) (oneBitNumber / (uint8_t) 8);
    for (uint8_t i = 0; i <= fullByteCount; i++) {
        target[i] = (uint8_t) (-1);
    }
    target[fullByteCount] = target[fullByteCount] << (8 - (oneBitNumber % 8));
}
