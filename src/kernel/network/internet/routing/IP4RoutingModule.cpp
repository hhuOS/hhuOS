//
// Created by hannes on 15.05.21.
//

#include <kernel/network/internet/IP4Interface.h>
#include <kernel/network/internet/IP4Module.h>

namespace Kernel {
    IP4RoutingModule::IP4RoutingModule() {
        this->routes = new Util::ArrayList<IP4Route *>();
    }

//Private method!
    uint8_t IP4RoutingModule::find(IP4Route **bestRoute, IP4Address *receiverAddress) {
        uint8_t matchingBits = 0, bestMatch = 0;
        *bestRoute = nullptr;

        if (receiverAddress == nullptr) {
            log.error("Given receiver address was null");
            return 1;
        }

        if (routes == nullptr) {
            log.error("Internal data structure for routes not initialized, not finding best one");
            return 1;
        }

        for (IP4Route *currentRoute:*this->routes) {
            if (currentRoute->matchingBits(&matchingBits, receiverAddress)) {
                log.error("Matching bits calculation failed, not finding best route");
                return 1;
            }
            if (matchingBits > 32) {
                log.error("matchingBits() function is broken");
                return 1;
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
        log.error("No route to host could be found");
        return 1;
    }

    uint8_t IP4RoutingModule::sendViaBestRoute(IP4Datagram *datagram) {
        if (datagram == nullptr) {
            log.error("Outgoing datagram was null, ignoring");
            return 1;
        }
        IP4Route *matchedRoute = nullptr;
        if (find(&matchedRoute, datagram->getDestinationAddress())) {
            log.error("Finding best route failed, return");
            //Datagram will be deleted in IP4Module
            //-> no delete here!
            return 1;
        }
        //Go to next level if everything worked fine
        return matchedRoute->sendOut(datagram);
    }

    void IP4RoutingModule::collectIP4RouteAttributes(Util::ArrayList<String> *strings) {
        if (strings == nullptr) {
            return;
        }
        if (routes == nullptr) {
            log.error("Internal data structure for routes not initialized, not collecting route attributes");
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
        if (ip4Interface == nullptr) {
            return;
        }
        if (routes == nullptr) {
            log.error("Internal data structure for routes not initialized, not adding route");
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
        if (routes == nullptr) {
            log.error("Internal data structure for routes not initialized, not removing route");
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
}