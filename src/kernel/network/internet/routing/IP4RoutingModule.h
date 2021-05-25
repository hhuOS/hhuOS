//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_IP4ROUTINGMODULE_H
#define HHUOS_IP4ROUTINGMODULE_H


#include "IP4Route.h"

class IP4RoutingModule {
private:
    Util::ArrayList<IP4Route *> *routes;
public:
    IP4RoutingModule();

    IP4Route *findRouteFor(IP4Address *receiverAddress);
};


#endif //HHUOS_IP4ROUTINGMODULE_H
