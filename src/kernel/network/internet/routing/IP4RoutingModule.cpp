//
// Created by hannes on 15.05.21.
//

#include <kernel/network/internet/IP4Interface.h>
#include "IP4RoutingModule.h"

IP4RoutingModule::IP4RoutingModule() {
    this->routes = new Util::ArrayList<IP4Route *>();
    this->defaultRoute= nullptr;
}

void IP4RoutingModule::setDefaultRoute(IP4Address *nextHop, IP4Interface *outInterface) {
    if(this->defaultRoute!= nullptr){
        delete this->defaultRoute;
        this->defaultRoute= nullptr;
    }
    this->defaultRoute=
        new IP4Route(
            new IP4Address(0,0,0,0),
            new IP4Netmask(0),
            nextHop,
            outInterface
        );
}

void IP4RoutingModule::addRouteForInterface(IP4Interface *ip4Interface) {
    //Add a direct route for a given new IP4Interface
    //-> Extract Network Address from interface's IP4Address with its Netmask
    //-> NextHop is null, we are directly connected here
    this->routes->add(
            new IP4Route(
                    ip4Interface->getIp4Netmask()->extractNetPart(ip4Interface->getIp4Address()),
                    ip4Interface->getIp4Netmask(), ip4Interface)
                    );
}

IP4Route *IP4RoutingModule::findRouteFor(IP4Address *receiverAddress) {
    uint8_t matchingBits,bestMatch = 0;
    IP4Route *bestRoute= nullptr;

    for(IP4Route *current:*this->routes){
        matchingBits=current->matchingBits(receiverAddress);
        if(matchingBits>32){
            //If matching bits calculation is broken return nullpointer
            //-> This will cause an ICMP4DestinationUnreachable message
            return nullptr;
        }
        if(matchingBits>bestMatch){
            bestRoute=current;
            bestMatch=matchingBits;
        }
    }
    //Return default route if no other route could be found
    //If no default route is set, return nullpointer
    //-> This will cause an ICMP4DestinationUnreachable message
    if(bestRoute== nullptr && this->defaultRoute != nullptr){
        return this->defaultRoute;
    }
    return bestRoute;
}
