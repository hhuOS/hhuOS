//
// Created by hannes on 14.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include <lib/string/String.h>
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

uint8_t IP4Address::copyTo(uint8_t *target) {
    if (address == nullptr || target == nullptr) {
        return 1;
    }
    for (int i = 0; i < IP4ADDRESS_LENGTH; i++) {
        target[i] = this->address[i];
    }
    return 0;
}

String IP4Address::asString() {
    if (address == nullptr) {
        return nullptr;
    }
    return String::format("'%d.%d.%d.%d'\0", address[0], address[1], address[2], address[3]);
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

uint8_t IP4Address::parseTo(uint8_t *targetBytes, char *input) {
    auto addressParts = String(input).split(".");
    if (addressParts.length() != IP4ADDRESS_LENGTH) {
        return 1;
    }

    int parsedValue;
    for (uint8_t i = 0; i < IP4ADDRESS_LENGTH; i++) {
        parsedValue = strtoint((const char *) addressParts[i]);
        if (parsedValue > 255) {
            return 1;
        }
        targetBytes[i] = (uint8_t) parsedValue;
    }
    return 0;
}

IP4Address *IP4Address::buildBroadcastAddress() {
    uint8_t allOnesBytes[IP4ADDRESS_LENGTH]{0xff, 0xff, 0xff, 0xff};
    return new IP4Address(allOnesBytes);
}