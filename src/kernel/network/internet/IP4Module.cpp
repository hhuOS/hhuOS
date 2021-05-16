//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/network/ethernet/EthernetAddress.h>
#include <kernel/core/System.h>
#include <kernel/service/EventBus.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include "IP4Module.h"

Kernel::IP4Module::IP4Module() {
    this->routingModule = new IP4RoutingModule();
    this->arpModule = new IP4ARPModule();
}

namespace Kernel {
    void Kernel::IP4Module::onEvent(const Kernel::Event &event) {
        if ((event.getType() == IP4SendEvent::TYPE)) {
            log.info("Received IP4 Datagram to be sent");
            IP4Datagram *datagram = ((IP4SendEvent &) event).getDatagram();
            IP4Address *destinationAddress = datagram->getDestinationAddress();

            IP4Route *matchedRoute = routingModule->findRouteFor(destinationAddress);
            auto *outInterface = matchedRoute->getOutInterface();
            auto *nextHop = matchedRoute->getNextHop();

//           datagram->setSourceAddress(outInterface->getIP4Address())
            EthernetAddress *destinationEthernetAddress = arpModule->resolveIP4(nextHop);
            if (destinationAddress == nullptr) {
                log.info("No ARP entry for IPv4 address found, ARP Request has been sent");
                return;
            }

            auto *outFrame = new EthernetFrame(destinationEthernetAddress, EthernetFrame::ETHERTYPE_IP4, datagram);
            auto *eventBus = Kernel::System::getService<Kernel::EventBus>();
            eventBus->publish(
                    Util::SmartPointer<Kernel::Event>(
                            new Kernel::EthernetSendEvent(outInterface, outFrame)
                    )
            );
        }
    }
}
