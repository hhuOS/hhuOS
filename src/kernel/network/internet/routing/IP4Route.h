//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4ROUTE_H
#define HHUOS_IP4ROUTE_H


#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/internet/addressing/IP4Netmask.h>
#include <kernel/network/internet/IP4Interface.h>
#include <device/network/NetworkDevice.h>

class IP4Route {

private:
    IP4Address *netAddress;
    IP4Netmask *netMask;
    IP4Address *nextHopAddress;
    IP4Interface *outInterface;

public:
    IP4Route(IP4Address *netAddress, IP4Netmask *netMask, IP4Address *nextHop, IP4Interface *outInterface);

    IP4Route(IP4Address *netAddress, IP4Netmask *netMask, IP4Interface *outInterface);

    IP4Route(IP4Interface *ip4Interface);

    void sendOut(IP4Datagram *datagram);

    IP4Address *getNextHopAddress() const;

    IP4Interface *getOutInterface() const;

    uint8_t matchingBits(IP4Address *pAddress);

    String asString();
};

#endif //HHUOS_IP4ROUTE_H
