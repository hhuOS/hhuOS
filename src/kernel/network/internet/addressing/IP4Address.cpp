//
// Created by hannes on 14.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "IP4Address.h"

IP4Address::IP4Address(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) {
    address = new uint8_t[IP4ADDRESS_LENGTH];
    address[0] = first;
    address[1] = second;
    address[2] = third;
    address[3] = fourth;
}

IP4Address::IP4Address(const uint8_t *bytes) {
    if (bytes != nullptr) {
        address = new uint8_t[IP4ADDRESS_LENGTH];
        for (int i = 0; i < IP4ADDRESS_LENGTH; i++) {
            this->address[i] = bytes[i];
        }
    }
}

IP4Address::IP4Address(IP4Address *other) {
    if (other != nullptr) {
        address = new uint8_t[IP4ADDRESS_LENGTH];
        other->copyTo(address);
    }
}

IP4Address::~IP4Address() {
    delete address;
}

bool IP4Address::equals(IP4Address *other) {
    if (address == nullptr || other == nullptr) {
        return false;
    }
    return
            (this->address[0] == other->address[0] &&
             this->address[1] == other->address[1] &&
             this->address[2] == other->address[2] &&
             this->address[3] == other->address[3]);
}

void IP4Address::copyTo(uint8_t *target) {
    if (address == nullptr || target == nullptr) {
        return;
    }
    for (int i = 0; i < IP4ADDRESS_LENGTH; i++) {
        target[i] = this->address[i];
    }
}

String IP4Address::asString() {
    if (address == nullptr) {
        return "NULL";
    }
    return String::format("%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
}

char *IP4Address::asChars() {
    return (char *) asString();
}

uint8_t IP4Address::calculateAND(IP4Address **ANDedAddress, const uint8_t *netmask) {
    if (netmask == nullptr || address == nullptr || ANDedAddress == nullptr) {
        return 1;
    }
    uint8_t bytes[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
    for (uint8_t i = 0; i < IP4ADDRESS_LENGTH; i++) {
        bytes[i] = (this->address[i] & netmask[i]);
    }
    *ANDedAddress = new IP4Address(bytes);
    return 0;
}
