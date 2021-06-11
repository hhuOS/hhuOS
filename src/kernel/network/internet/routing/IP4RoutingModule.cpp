//
// Created by hannes on 15.05.21.
//

#include <kernel/network/internet/IP4Interface.h>
#include <kernel/network/internet/IP4Module.h>

IP4RoutingModule::IP4RoutingModule() {
    this->routes = new Util::ArrayList<IP4Route *>();
    this->defaultRoute = nullptr;
}

//Private method!
uint8_t IP4RoutingModule::find(IP4Route **bestRoute, IP4Address *receiverAddress) {
    uint8_t matchingBits, bestMatch = 0;
    *bestRoute = nullptr;

    if (receiverAddress == nullptr) {
        return IP4_RECEIVER_ADDRESS_NULL;
    }

    for (IP4Route *currentRoute:*this->routes) {
        matchingBits = currentRoute->matchingBits(receiverAddress);
        if (matchingBits > 32) {
            return IP4_MATCHING_BITS_FUNCTION_BROKEN;
        }
        if (matchingBits > bestMatch) {
            *bestRoute = currentRoute;
            bestMatch = matchingBits;
        }
    }

    //Return successful if best route found
    if (*bestRoute != nullptr) {
        return 0;
    }

    //Set to default route if it exists and return successful
    if (this->defaultRoute != nullptr) {
        *bestRoute = this->defaultRoute;
        return 0;
    }

    //Return error if no route could be found at all
    return IP4_NO_ROUTE_FOUND;
}

uint8_t IP4RoutingModule::sendViaBestRoute(IP4Datagram *datagram) {
    if (datagram == nullptr) {
        return IP4_DATAGRAM_NULL;
    }
    IP4Route *matchedRoute = nullptr;
    uint8_t findError = find(&matchedRoute, datagram->getDestinationAddress());
    if (findError) {
        return findError;
    }
    //Go to next level if everything worked fine
    return matchedRoute->sendOut(datagram);
}

void IP4RoutingModule::collectIP4RouteAttributes(Util::ArrayList<String> *strings) {
    if (strings == nullptr) {
        return;
    }
    for (IP4Route *current:*this->routes) {
        strings->add(current->asString());
    }
}

[[maybe_unused]] void IP4RoutingModule::setDefaultRoute(IP4Address *nextHop, IP4Interface *outInterface) {
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
    if (ip4Interface == nullptr) {
        return;
    }
    //Add a direct route for a given new IP4Interface
    //-> Extract Network Address from interface's IP4Address with its Netmask
    //-> NextHop is null, we are directly connected here
    this->routes->add(new IP4Route(ip4Interface));
}

void IP4RoutingModule::removeRoutesFor(IP4Interface *ip4Interface) {
    if (ip4Interface == nullptr) {
        return;
    }
    //TODO: Synchronization!
    for (uint32_t i = 0; i < routes->size(); i++) {
        if (routes->get(i)->getOutInterface()->equals(ip4Interface)) {
            routes->remove(i);
            i--;
        }
    }
}
