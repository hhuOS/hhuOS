//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4ROUTE_H
#define HHUOS_IP4ROUTE_H


#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/internet/addressing/IP4Netmask.h>
#include <device/network/NetworkDevice.h>

class IP4Route {

private:
    IP4Address *netAddress;
    IP4Netmask *netMask;
    IP4Address *nextHopAddress;
    NetworkDevice *outInterface;

public:
    IP4Route(IP4Address *netAddress, IP4Netmask *netMask, IP4Address *nextHop, NetworkDevice *outInterface);

    IP4Address *getNetAddress() const;

    IP4Netmask *getNetMask() const;

    IP4Address *getNextHopAddress() const;

    NetworkDevice *getOutInterface() const;
};


#endif //HHUOS_IP4ROUTE_H
