//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/IP4SendEvent.h>
#include "IP4Module.h"

Kernel::IP4Module::IP4Module() {
    this->routingModule = new IP4RoutingModule();
}

namespace Kernel {
    void Kernel::IP4Module::onEvent(const Kernel::Event &event) {
        if ((event.getType() == IP4SendEvent::TYPE)) {
            log.info("Received IP4 Datagram to be sent");
            IP4Datagram *datagram=((IP4SendEvent &) event).getDatagram();
            IP4Route *matchedRoute=routingModule->findRouteFor(datagram->getDestinationAddress());
//            datagram->setSourceAddress(matchedRoute->getOutInterface()->getIP4Address())

        }
    }
}
