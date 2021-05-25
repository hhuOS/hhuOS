//
// Created by hannes on 15.05.21.
//

#include <kernel/network/internet/IP4Interface.h>
#include "IP4RoutingModule.h"

IP4RoutingModule::IP4RoutingModule() {
    this->routes=new Util::ArrayList<IP4Route *>();
}

IP4Route *IP4RoutingModule::findRouteFor(IP4Address *receiverAddress) {
    return routes->get(0);
    //TODO: Implement pathfinding
}

void IP4RoutingModule::addRouteForInterface(IP4Interface *ip4Interface) {
    routes->add(
            new IP4Route(ip4Interface->getIp4Address(),ip4Interface->getIp4Netmask(),ip4Interface));
}
