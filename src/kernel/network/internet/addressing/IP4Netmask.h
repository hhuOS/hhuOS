//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4NETMASK_H
#define HHUOS_IP4NETMASK_H

#include "lib/libc/stdlib.h"
#include "IP4Address.h"

class IP4Netmask {
private:
    uint8_t *netmask = nullptr;
    uint8_t bitCount = 0;

public:
    explicit IP4Netmask(uint8_t bitCount);

    [[nodiscard]] uint8_t getBitCount() const;

    String asString();

    IP4Address *extractNetPart(IP4Address *ip4Address);

    static void calculateBitmask(uint8_t *target, uint8_t oneBitNumber);

    virtual ~IP4Netmask();
};


#endif //HHUOS_IP4NETMASK_H
