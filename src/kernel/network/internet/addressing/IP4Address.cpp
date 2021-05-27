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

uint32_t IP4Address::asInt() {
    return (uint32_t) *address;
}

IP4Address::IP4Address(uint32_t fullAddress) {
    auto resultBytes = (uint8_t *) &fullAddress;
    for (int i = 0; i < 4; i++) {
        this->address[i] = resultBytes[i];
    }
}

uint8_t IP4Address::equals(IP4Address *ip4Address) {
    return ip4Address->asInt() == this->asInt();
}
