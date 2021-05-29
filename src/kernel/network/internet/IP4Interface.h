//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_IP4INT_H
#define HHUOS_IP4INT_H


#include <kernel/network/internet/arp/ARPModule.h>
#include <kernel/network/ethernet/EthernetDevice.h>
#include <kernel/service/EventBus.h>
#include <kernel/network/internet/addressing/IP4Netmask.h>
#include "IP4Datagram.h"

class IP4Interface {
private:
    ARPModule *arpModule;
    Kernel::EventBus *eventBus;
    IP4Address *ip4Address;
    IP4Netmask *ip4Netmask;
    EthernetDevice *ethernetDevice;

public:

    IP4Interface(Kernel::EventBus *eventBus, EthernetDevice *ethernetDevice, IP4Address *ip4Address,
                 IP4Netmask *ip4Netmask);

    void sendIP4Datagram(IP4Address *receiver, IP4Datagram *ip4Datagram);

    IP4Address *getIp4Address() const;

    IP4Netmask *getIp4Netmask() const;

    IP4Address *getNetAddress() const;

    virtual ~IP4Interface();

    bool equals(IP4Interface *compare);

    String asString();
};

#endif //HHUOS_IP4INT_H
