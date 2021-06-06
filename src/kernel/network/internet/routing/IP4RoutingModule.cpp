//
// Created by hannes on 15.05.21.
//

#include <kernel/network/internet/IP4Interface.h>
#include "IP4RoutingModule.h"

IP4RoutingModule::IP4RoutingModule() {
    this->routes = new Util::ArrayList<IP4Route *>();
    this->defaultRoute = nullptr;
}

//Private method!
IP4Route *IP4RoutingModule::findBestRouteFor(IP4Address *receiverAddress) {
    uint8_t matchingBits, bestMatch = 0;
    IP4Route *bestRoute = nullptr;

    if(receiverAddress== nullptr){
        return nullptr;
    }

    for (IP4Route *current:*this->routes) {
        matchingBits = current->matchingBits(receiverAddress);
        if (matchingBits > 32) {
            //If matching bits calculation is broken return nullpointer
            //-> This will cause an ICMP4DestinationUnreachable message
            return nullptr;
        }
        if (matchingBits > bestMatch) {
            bestRoute = current;
            bestMatch = matchingBits;
        }
    }
    //Return default route if no other route could be found
    //If no default route is set, return nullpointer
    //-> This will cause an ICMP4DestinationUnreachable message
    if (bestRoute == nullptr && this->defaultRoute != nullptr) {
        return this->defaultRoute;
    }
    return bestRoute;
}

uint8_t IP4RoutingModule::sendViaBestRoute(IP4Datagram *datagram) {
    if(datagram== nullptr){
        return 1;
    }
    //TODO: Return specific error codes depending on what happened! (Like 1=NO_ROUTE_FOUND etc.)
    IP4Route *matchedRoute = this->findBestRouteFor(datagram->getDestinationAddress());
    if (matchedRoute == nullptr) {
        return 1;
    }
    //Go to next level if everything worked fine
    return matchedRoute->sendOut(datagram);
}

void IP4RoutingModule::collectIP4RouteAttributes(Util::ArrayList<String> *strings) {
    if(strings == nullptr){
        return;
    }
    for (IP4Route *current:*this->routes) {
        strings->add(current->asString());
    }
}

void IP4RoutingModule::setDefaultRoute(IP4Address *nextHop, IP4Interface *outInterface) {
    if (this->defaultRoute != nullptr) {
        delete this->defaultRoute;
        this->defaultRoute = nullptr;
    }
    this->defaultRoute =
            new IP4Route(
                    new IP4Address(0, 0, 0, 0),
                    new IP4Netmask(0),
                    nextHop,
                    outInterface
            );
}

void IP4RoutingModule::addRouteFor(IP4Interface *ip4Interface) {
    if(ip4Interface== nullptr){
        return;
    }
    //Add a direct route for a given new IP4Interface
    //-> Extract Network Address from interface's IP4Address with its Netmask
    //-> NextHop is null, we are directly connected here
    this->routes->add(new IP4Route(ip4Interface));
}

void IP4RoutingModule::removeRoutesFor(IP4Interface *ip4Interface) {
    if(ip4Interface== nullptr){
        return;
    }
    //TODO: Synchronisierung!
    for (uint32_t i = 0; i < routes->size(); i++) {
        if (routes->get(i)->getOutInterface()->equals(ip4Interface)) {
            routes->remove(i);
            i--;
        }
    }
}
