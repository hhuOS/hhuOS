//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include <lib/string/String.h>
#include "IP4Netmask.h"

IP4Netmask::IP4Netmask(uint8_t bitCount) {
    uint8_t maxBitCount = IP4ADDRESS_LENGTH * 8;
    netmask = new uint8_t[IP4ADDRESS_LENGTH];

    if (bitCount > maxBitCount) {
        bitCount = maxBitCount;
    }
    this->bitCount = bitCount;

    if (bitCount == 0) {
        netmask[0] = 0;
        netmask[1] = 0;
        netmask[2] = 0;
        netmask[3] = 0;
        return;
    }

    uint32_t base = 0xffffffff;
    base = base << (32 - bitCount);

    //byte order...
    auto *baseAsBytes = (uint8_t *) &base;
    netmask[3] = baseAsBytes[0];
    netmask[2] = baseAsBytes[1];
    netmask[1] = baseAsBytes[2];
    netmask[0] = baseAsBytes[3];
}

IP4Netmask::~IP4Netmask() {
    delete netmask;
}

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
        return nullptr;
    }
    return String::format("'%d.%d.%d.%d /%d'", netmask[0], netmask[1], netmask[2], netmask[3], bitCount);
}