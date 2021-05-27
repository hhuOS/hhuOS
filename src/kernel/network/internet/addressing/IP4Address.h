//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4ADDRESS_H
#define HHUOS_IP4ADDRESS_H

#include <cinttypes>
#include <lib/string/String.h>

class IP4Address {
private:
    uint8_t address[4]{0, 0, 0, 0};

public:
    IP4Address(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);

    IP4Address(uint32_t fullAddress);

    uint32_t asInt();

    String asString();

    uint8_t equals(IP4Address *ip4Address);
};


#endif //HHUOS_IP4ADDRESS_H
