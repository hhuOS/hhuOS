//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/network/ethernet/EthernetAddress.h>
#include <kernel/core/System.h>
#include <kernel/service/EventBus.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/network/internet/arp/ARPRequest.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/network/internet/icmp/messages/ICMP4DestinationUnreachable.h>
#include <kernel/event/network/UDPReceiveEvent.h>
#include "IP4Module.h"

Kernel::IP4Module::IP4Module() {
    this->eventBus = Kernel::System::getService<Kernel::EventBus>();
    this->routingModule = new IP4RoutingModule();
    this->arpModule = new ARPModule();
}

namespace Kernel {
    IP4Module::~IP4Module() {
        delete routingModule;
        delete arpModule;
    }

    void Kernel::IP4Module::onEvent(const Kernel::Event &event) {
        if ((event.getType() == IP4SendEvent::TYPE)) {
            log.info("Received IP4 Datagram to be sent");
            IP4Datagram *datagram = ((IP4SendEvent &) event).getDatagram();
            IP4Address *destinationAddress = datagram->getDestinationAddress();

            IP4Route *matchedRoute = routingModule->findRouteFor(destinationAddress);
            if(matchedRoute== nullptr){
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::ICMP4ReceiveEvent(
                                        new ICMP4DestinationUnreachable()
                                        )
                        )
                );
                return;
            }

            auto *outInterface = matchedRoute->getOutInterface();
            auto *nextHopAddress = matchedRoute->getNextHopAddress();

//           datagram->setSourceAddress(outInterface->getIP4Address())
            EthernetAddress *destinationEthernetAddress = arpModule->resolveIP4(nextHopAddress);
            if (destinationAddress == nullptr) {
                log.info("No ARP entry for IPv4 address found, sending ARP Request");
                auto *arpRequest = new ARPRequest(nextHopAddress);
                auto *outFrame = new EthernetFrame(destinationEthernetAddress, arpRequest);
                //TODO: Implement data structure for waiting IP4Datagrams
                eventBus->publish(
                        Util::SmartPointer<Kernel::Event>(
                                new Kernel::EthernetSendEvent(outInterface, outFrame)
                        )
                );
                return;
            }

            auto *outFrame = new EthernetFrame(destinationEthernetAddress, datagram);
            eventBus->publish(
                    Util::SmartPointer<Kernel::Event>(
                            new Kernel::EthernetSendEvent(outInterface, outFrame)
                    )
            );
            return;
        }
        if (event.getType() == IP4ReceiveEvent::TYPE) {
            auto *ip4Datagram = ((IP4ReceiveEvent &) event).getDatagram();
            switch (ip4Datagram->getIp4ProtocolType()) {
                case IP4ProtocolType::ICMP4:
                    eventBus->publish(
                            Util::SmartPointer<Kernel::Event>(
                                    new Kernel::ICMP4ReceiveEvent(ip4Datagram->getIp4DataPart())
                            )
                    );
                    break;
                case IP4ProtocolType::UDP:
                    eventBus->publish(
                            Util::SmartPointer<Kernel::Event>(
                                    new Kernel::UDPReceiveEvent(
                                            new UDPDatagram(
                                                        ip4Datagram->getIp4DataPart()
                                                    )
                                            )
                            )
                    );
                    break;
            }
            return;
        }
        if (event.getType() == ARPReceiveEvent::TYPE) {
            auto *arpResponse = ((ARPReceiveEvent &) event).getArpResponse();
            arpModule->addEntry(arpResponse->getIp4Address(), arpResponse->getEthernetAddress());
            //TODO: Add check for waiting IP4Datagrams and send them again
            return;
        }
    }
}
