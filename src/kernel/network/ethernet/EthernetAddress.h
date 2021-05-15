//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETADDRESS_H
#define HHUOS_ETHERNETADDRESS_H


#include <cinttypes>

class EthernetAddress {
private:
    uint8_t macAddress[6];
public:
    EthernetAddress();
};


#endif //HHUOS_ETHERNETADDRESS_H
