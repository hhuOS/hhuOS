//
// Created by hannes on 29.05.21.
//

#ifndef HHUOS_ARPENTRY_H
#define HHUOS_ARPENTRY_H


#include <kernel/network/internet/IP4Address.h>
#include <kernel/network/ethernet/EthernetAddress.h>

class ARPEntry {
private:
    IP4Address *ip4Address;
    EthernetAddress *ethernetAddress;

public:
    ARPEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress);

    virtual ~ARPEntry();

    bool matches(IP4Address *otherAddress);

    EthernetAddress *getEthernetAddress();

    void setEthernetAddress(EthernetAddress *otherAddress);

    String asString();
};


#endif //HHUOS_ARPENTRY_H
