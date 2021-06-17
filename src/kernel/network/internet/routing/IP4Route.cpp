//
// Created by hannes on 15.05.21.
//

#include <kernel/network/internet/IP4Module.h>

IP4Route::IP4Route(IP4Address *netAddress, IP4Netmask *netMask, IP4Address *nextHop,
                   Kernel::IP4Interface *outInterface) {
    this->netAddress = netAddress;
    this->netMask = netMask;
    this->nextHopAddress = nextHop;
    this->outInterface = outInterface;
}

IP4Route::IP4Route(IP4Address *netAddress, IP4Netmask *netMask, Kernel::IP4Interface *outInterface) {
    this->netAddress = netAddress;
    this->netMask = netMask;
    this->nextHopAddress = nullptr;
    this->outInterface = outInterface;
}

uint8_t IP4Route::sendOut(IP4Datagram *datagram) {
    if (datagram == nullptr || outInterface == nullptr) {
        return 1;
    }
    if (nextHopAddress == nullptr) {
        //direct route
        return outInterface->sendIP4Datagram(datagram->getDestinationAddress(), datagram);
    }
    //indirect route
    return outInterface->sendIP4Datagram(nextHopAddress, datagram);
}

Kernel::IP4Interface *IP4Route::getOutInterface() const {
    return outInterface;
}

uint8_t IP4Route::matchingBits(IP4Address *ip4Address) {
    IP4Address *addressNetPart = netMask->extractNetPart(ip4Address);
    if (addressNetPart->equals(netAddress)) {
        return netMask->getBitCount();
    }
    return 0;
}

IP4Route::IP4Route(Kernel::IP4Interface *ip4Interface) {
    netAddress = ip4Interface->getNetAddress();
    netMask = ip4Interface->getIp4Netmask();
    nextHopAddress = nullptr;
    outInterface = ip4Interface;
}

String IP4Route::asString() {
    if (nextHopAddress == nullptr) {
        return "\nNetAddress: " + netAddress->asString() + ",\nNetMask: " + netMask->asString() + ",\nNextHop: null" +
               ",\nOutInterface: " + outInterface->asString();
    }
    return "\nNetAddress: " + netAddress->asString() + ",\nNetMask: " + netMask->asString() + ",\nNextHop: " +
           nextHopAddress->asString() + ",\nOutInterface: " + outInterface->asString();
}
