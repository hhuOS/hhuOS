//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETADDRESS_H
#define HHUOS_ETHERNETADDRESS_H


#include <cinttypes>
#include <lib/string/String.h>

class EthernetAddress {
private:
    uint8_t macAddress[6]{0, 0, 0, 0, 0, 0};
public:
    EthernetAddress(
            uint8_t firstByte, uint8_t secondByte, uint8_t thirdByte,
            uint8_t fourthByte, uint8_t fifthByte, uint8_t sixthByte);

    String asString();
};


#endif //HHUOS_ETHERNETADDRESS_H
