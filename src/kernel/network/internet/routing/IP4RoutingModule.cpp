//
// Created by hannes on 15.05.21.
//

#include "IP4RoutingModule.h"

namespace Kernel {
    //Private method!
    uint8_t IP4RoutingModule::find(IP4Route **bestRoute, IP4Address *receiverAddress) {
        if (routes == nullptr || accessLock == nullptr) {
            log.error("Route table or access lock not initialized, not finding best one");
            return 1;
        }
        uint8_t matchingBits = 0, bestMatch = 0;
        *bestRoute = nullptr;
        accessLock->acquire();
        for (IP4Route *currentRoute:*routes) {
            if (currentRoute->matchingBits(&matchingBits, receiverAddress)) {
                log.error("Matching bits calculation failed, not finding best route");
                accessLock->release();
                return 1;
            }
            if (matchingBits > 32) {
                log.error("matchingBits() function is broken");
                accessLock->release();
                return 1;
            }
            if (matchingBits > bestMatch) {
                *bestRoute = currentRoute;
                bestMatch = matchingBits;
            }
        }
        accessLock->release();
        //If a good route has been found
        if (*bestRoute != nullptr) {
            return 0;
        }
        log.info("No route for %s has been found, try default route", receiverAddress->asChars());
        if (defaultRoute != nullptr) {
            log.info("Default route has been set, interface: %s",
                     (char *) defaultRoute->getOutInterface()->asString());
            *bestRoute = defaultRoute;
            return 0;
        }
        log.error("No route to host %s could be found", receiverAddress->asChars());
        return 1;
    }

    IP4RoutingModule::IP4RoutingModule() {
        routes = new Util::ArrayList<IP4Route *>();
        accessLock = new Spinlock();
        accessLock->release();
    }

    IP4RoutingModule::~IP4RoutingModule() {
        if (routes != nullptr) {
            IP4Route *toDelete;
            for (uint32_t i = 0; i < routes->size(); i++) {
                toDelete = routes->get(i);
                routes->remove(i);
                i--;
                delete toDelete;
            }
        }
        delete accessLock;
        delete defaultRoute;
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

    uint8_t IP4RoutingModule::collectIP4RouteAttributes(Util::ArrayList<String> *strings) {
        if (strings == nullptr) {
            return 1;
        }
        if (routes == nullptr || accessLock == nullptr) {
            log.error("Route table or access lock not initialized, not collecting route attributes");
            return 1;
        }
        accessLock->acquire();
        if (defaultRoute != nullptr) {
            strings->add(defaultRoute->asString());
        }
        for (IP4Route *current:*routes) {
            strings->add(current->asString());
        }
        accessLock->release();
        return 0;
    }

    uint8_t
    IP4RoutingModule::addDirectRouteFor(IP4Address *netAddress, IP4Netmask *netMask, IP4Interface *outInterface) {
        if (routes == nullptr || accessLock == nullptr) {
            log.error("Route table or access lock not initialized, not adding direct route");
            return 1;
        }
        if (netAddress == nullptr || netMask == nullptr || outInterface == nullptr) {
            log.error("At least one parameter was null, not adding direct route");
            return 1;
        }
        accessLock->acquire();
        routes->add(new IP4Route(netAddress, netMask, outInterface));
        accessLock->release();
        return 0;
    }

    uint8_t IP4RoutingModule::removeRoutesFor(IP4Interface *ip4Interface) {
        if (ip4Interface == nullptr) {
            return 1;
        }
        if (routes == nullptr || accessLock == nullptr) {
            log.error("Route table or access lock not initialized, not removing route");
            return 1;
        }
        accessLock->acquire();
        for (uint32_t i = 0; i < routes->size(); i++) {
            if (routes->get(i)->getOutInterface()->equals(ip4Interface)) {
                routes->remove(i);
                i--;
            }
        }
        accessLock->release();
        return 0;
    }

    uint8_t IP4RoutingModule::setDefaultRoute(IP4Address *gatewayAddress, IP4Interface *outInterface) {
        if (gatewayAddress == nullptr || outInterface == nullptr) {
            log.error("Gateway address or out interface was null, not setting default route");
            return 1;
        }
        if (accessLock == nullptr) {
            log.error("Access lock not initialized, not setting default route");
            return 1;
        }
        accessLock->acquire();
        delete defaultRoute;
        auto *defaultAddress = new IP4Address(0, 0, 0, 0);
        auto *defaultNetmask = new IP4Netmask(0);
        defaultRoute = new IP4Route(defaultAddress, defaultNetmask, gatewayAddress, outInterface);
        accessLock->release();
        return 0;
    }

    uint8_t IP4RoutingModule::removeDefaultRoute() {
        if (accessLock == nullptr) {
            log.error("Access lock not initialized, not removing default route");
            return 1;
        }
        accessLock->acquire();
        delete defaultRoute;
        defaultRoute = nullptr;
        accessLock->release();
        return 0;
    }
}