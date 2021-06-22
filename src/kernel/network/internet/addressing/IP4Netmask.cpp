//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "IP4Netmask.h"

IP4Netmask::IP4Netmask(uint8_t bitCount) {
    uint8_t maxBitCount = IP4ADDRESS_LENGTH * 8;
    netmask = new uint8_t[IP4ADDRESS_LENGTH];

    if (bitCount > maxBitCount) {
        bitCount = maxBitCount;
    }
    this->bitCount = bitCount;
    calculateBitmask(this->netmask, bitCount);
}

IP4Netmask::~IP4Netmask() {
    delete netmask;
}

//NOTE: This method creates a new IP4Address!
//-> don't forget to delete it after usage!
uint8_t IP4Netmask::extractNetPart(IP4Address **targetNetPart, IP4Address *ip4Address) {
    if (this->netmask == nullptr || ip4Address == nullptr || targetNetPart == nullptr) {
        return 1;
    }
    return ip4Address->calculateAND(targetNetPart, this->netmask);
}

uint8_t IP4Netmask::getBitCount() const {
    return this->bitCount;
}

String IP4Netmask::asString() {
    if (netmask == nullptr) {
        return "NULL";
    }
    return String::format("%d.%d.%d.%d /%d", netmask[0], netmask[1], netmask[2], netmask[3], bitCount);
}

void IP4Netmask::calculateBitmask(uint8_t *target, uint8_t oneBitNumber) {
    if (target == nullptr) {
        return;
    }
    uint8_t byteSize = 8;
    uint8_t fullByteCount = oneBitNumber / byteSize;
    for (uint8_t i = 0; i <= fullByteCount; i++) {
        target[i] = 0xff;
    }
    target[fullByteCount] = target[fullByteCount] << (8 - (oneBitNumber % 8));
}
