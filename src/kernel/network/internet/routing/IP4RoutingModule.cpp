//
// Created by hannes on 15.05.21.
//

#include "IP4RoutingModule.h"

IP4Route *IP4RoutingModule::findRouteFor(IP4Address *receiverAddress) {
    return new IP4Route(nullptr, nullptr, nullptr, nullptr);
}