//
// Created by hannes on 29.05.21.
//

#ifndef HHUOS_ARPENTRY_H
#define HHUOS_ARPENTRY_H


#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/ethernet/EthernetAddress.h>

class ARPEntry {
private:
    IP4Address *ip4Address;
    EthernetAddress *ethernetAddress;

public:
    ARPEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress);

    bool matches(IP4Address *otherAddress);

    EthernetAddress *getEthernetAddress();
};


#endif //HHUOS_ARPENTRY_H
