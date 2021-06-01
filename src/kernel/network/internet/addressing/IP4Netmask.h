//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4NETMASK_H
#define HHUOS_IP4NETMASK_H

#include "lib/libc/stdlib.h"
#include "IP4Address.h"

class IP4Netmask {
private:
    uint8_t netmask[4]{0,0,0,0};
    uint8_t bitCount = 0;

public:
    IP4Netmask(uint8_t bitCount);

    uint8_t getBitCount() const;

    String asString();

    IP4Address *extractNetPart(IP4Address *ip4Address);
};


#endif //HHUOS_IP4NETMASK_H
