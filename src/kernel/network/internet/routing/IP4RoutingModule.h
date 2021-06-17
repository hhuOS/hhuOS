//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4ROUTINGMODULE_H
#define HHUOS_IP4ROUTINGMODULE_H


#include <kernel/network/internet/addressing/IP4Address.h>
#include "IP4Route.h"

namespace Kernel {
    class IP4RoutingModule {
    private:
        Util::ArrayList<IP4Route *> *routes = nullptr;
        IP4Route *defaultRoute = nullptr;

        uint8_t find(IP4Route **bestRoute, IP4Address *receiverAddress);

        /**
                 * A logger to provide information on the kernel log.
                 */
        Logger &log = Logger::get("IP4RoutingModule");
    public:

        IP4RoutingModule();

        void addRouteFor(IP4Interface *ip4Interface);

        void setDefaultRoute(IP4Address *nextHop, IP4Interface *outInterface);

        void removeRoutesFor(IP4Interface *ip4Interface);

        void collectIP4RouteAttributes(Util::ArrayList<String> *strings);

        uint8_t sendViaBestRoute(IP4Datagram *datagram);
    };
}

#endif //HHUOS_IP4ROUTINGMODULE_H
