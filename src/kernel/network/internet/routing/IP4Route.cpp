//
// Created by hannes on 15.05.21.
//

#include "IP4Route.h"

IP4Route::IP4Route(IP4Address *netAddress, IP4Netmask *netMask, IP4Address *nextHop, NetworkDevice *outInterface)
        : netAddress(netAddress), netMask(netMask), nextHop(nextHop), outInterface(outInterface) {}

IP4Address *IP4Route::getNetAddress() const {
    return netAddress;
}

IP4Netmask *IP4Route::getNetMask() const {
    return netMask;
}

IP4Address *IP4Route::getNextHop() const {
    return nextHop;
}

NetworkDevice *IP4Route::getOutInterface() const {
    return outInterface;
}
