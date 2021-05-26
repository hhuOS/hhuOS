//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4ROUTINGMODULE_H
#define HHUOS_IP4ROUTINGMODULE_H


#include <kernel/network/internet/addressing/IP4Address.h>
#include "IP4Route.h"

class IP4RoutingModule {
private:
    Util::ArrayList<IP4Route *> *routes;
    IP4Route *defaultRoute;
public:
    IP4RoutingModule();

    void addRouteForInterface(IP4Interface *ip4Interface);

    void setDefaultRoute(IP4Address *nextHop,IP4Interface *outInterface);

    IP4Route *findRouteFor(IP4Address *receiverAddress);
};

#endif //HHUOS_IP4ROUTINGMODULE_H
