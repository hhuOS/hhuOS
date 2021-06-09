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

IP4Address::IP4Address(const uint8_t *bytes) {
    for (int i = 0; i < IP4ADDRESS_LENGTH; i++) {
        this->address[i] = bytes[i];
    }
}

bool IP4Address::equals(IP4Address *other) {
    return
            (this->address[0] == other->address[0] &&
             this->address[1] == other->address[1] &&
             this->address[2] == other->address[2] &&
             this->address[3] == other->address[3]);
}

void IP4Address::copyTo(uint8_t *target) {
    for (int i = 0; i < IP4ADDRESS_LENGTH; i++) {
        target[i] = this->address[i];
    }
}

String IP4Address::asString() {
    return String::format("%d.%d.%d.%d", address[0], address[1], address[2], address[3]);
}

IP4Address *IP4Address::calculateAND(const uint8_t *netmask) {
    uint8_t bytes[IP4ADDRESS_LENGTH]{0,0,0,0};
    for (uint8_t i = 0; i < IP4ADDRESS_LENGTH; i++) {
        bytes[i] = (this->address[i] & netmask[i]);
    }
    return new IP4Address(bytes);
}
