//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4ADDRESS_H
#define HHUOS_IP4ADDRESS_H

#include <cinttypes>

class IP4Address {
private:
    uint8_t address[4]{0,0,0,0};

public:
    IP4Address(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);
};


#endif //HHUOS_IP4ADDRESS_H
