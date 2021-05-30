//
// Created by hannes on 15.05.21.
//

#include "IP4Route.h"

IP4Route::IP4Route(IP4Address *netAddress, IP4Netmask *netMask, IP4Address *nextHop,
                   IP4Interface *outInterface) {
    this->netAddress = netAddress;
    this->netMask = netMask;
    this->nextHopAddress = nextHop;
    this->outInterface = outInterface;
}

IP4Route::IP4Route(IP4Address *netAddress, IP4Netmask *netMask, IP4Interface *outInterface) {
    this->netAddress = netAddress;
    this->netMask = netMask;
    this->nextHopAddress = nullptr;
    this->outInterface = outInterface;
}

void IP4Route::sendOut(IP4Datagram *datagram) {
    if (this->nextHopAddress == nullptr) {
        this->outInterface->sendIP4Datagram(datagram->getDestinationAddress(), datagram);
        return;
    }
    this->outInterface->sendIP4Datagram(this->nextHopAddress, datagram);
}

IP4Address *IP4Route::getNextHopAddress() const {
    return nextHopAddress;
}

IP4Interface *IP4Route::getOutInterface() const {
    return outInterface;
}

uint8_t IP4Route::matchingBits(IP4Address *ip4Address) {
    IP4Address *addressNetPart = this->netMask->extractNetPart(ip4Address);
    if (addressNetPart->equals(this->netAddress)) {
        return this->netMask->getBitCount();
    }
    return 0;
}

IP4Route::IP4Route(IP4Interface *ip4Interface) {
    this->netAddress = ip4Interface->getNetAddress();
    this->netMask = ip4Interface->getIp4Netmask();
    this->nextHopAddress = nullptr;
    this->outInterface = ip4Interface;
}

String IP4Route::asString() {
    if (nextHopAddress == nullptr) {
        return "\nNetAddress: " + netAddress->asString() + ",\nNetMask: " + netMask->asString() + ",\nNextHop: null" +
               ",\nOutInterface: " + outInterface->asString();
    }
    return "\nNetAddress: " + netAddress->asString() + ",\nNetMask: " + netMask->asString() + ",\nNextHop: " +
           nextHopAddress->asString() + ",\nOutInterface: " + outInterface->asString();
}
