//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4ADDRESS_H
#define HHUOS_IP4ADDRESS_H

#include <cstdint>
#include <lib/string/String.h>

class IP4Address {
private:
    uint8_t *address;

public:
    IP4Address(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);

    explicit IP4Address(const uint8_t *bytes);

    explicit IP4Address(IP4Address *other);

    virtual ~IP4Address();

    static IP4Address *buildBroadcastAddress();

    bool equals(IP4Address *other);

    uint8_t copyTo(uint8_t *target);

    uint8_t calculateAND(IP4Address **ANDedAddress, const uint8_t *netmask);

    static uint8_t parseTo(uint8_t *targetBytes, char *input);

    String asString();
};


#endif //HHUOS_IP4ADDRESS_H
