//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4NETMASK_H
#define HHUOS_IP4NETMASK_H

#include <cinttypes>

class IP4Netmask {
private:
    uint8_t netmask[4];

public:
    IP4Netmask(uint8_t first,uint8_t second,uint8_t third,uint8_t fourth);
};


#endif //HHUOS_IP4NETMASK_H
