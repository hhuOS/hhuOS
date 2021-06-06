//
// Created by hannes on 14.05.21.
//

#include "IP4Module.h"

namespace Kernel {
    IP4Module::IP4Module(NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        this->routingModule = new IP4RoutingModule();
        this->interfaces = new Util::HashMap<EthernetDevice *, IP4Interface *>();
    }

    IP4Module::~IP4Module() {
        delete routingModule;
    }

    void IP4Module::collectIP4InterfaceAttributes(Util::ArrayList<String> *strings) {
        for (EthernetDevice *currentDevice:interfaces->keySet()) {
            strings->add(interfaces->get(currentDevice)->asString());
        }
    }

    void IP4Module::collectIP4RouteAttributes(Util::ArrayList<String> *strings) {
        this->routingModule->collectIP4RouteAttributes(strings);
    }

    void IP4Module::registerDevice(EthernetDevice *device, IP4Address *ip4Address, IP4Netmask *ip4Netmask) {
        if (device == nullptr || ip4Address == nullptr || ip4Netmask == nullptr) {
            log.error("At least one given parameter was null, not registering new device");
            return;
        }
        if (interfaces->containsKey(device)) {
            log.error("Ethernet device already registered, not registering it");
            return;
        }
        auto *newInterface = new IP4Interface(eventBus, device, ip4Address, ip4Netmask);
        interfaces->put(device, newInterface);
        routingModule->addRouteFor(newInterface);
    }

    void IP4Module::unregisterDevice(EthernetDevice *device) {
        if (device == nullptr) {
            log.error("Given device was null, not unregistering anything");
            return;
        }
        if (interfaces->containsKey(device)) {
            routingModule->removeRoutesFor(interfaces->get(device));
            interfaces->remove(device);
        }
    }

    void IP4Module::onEvent(const Kernel::Event &event) {
        if ((event.getType() == IP4SendEvent::TYPE)) {
            IP4Datagram *datagram = ((IP4SendEvent &) event).getDatagram();

            if (routingModule->sendViaBestRoute(datagram)) {
                eventBus->publish(
                        new Kernel::ICMP4ReceiveEvent(
                                new ICMP4DestinationUnreachable()
                        )
                );
            }
            return;
        }

        if ((event.getType() == IP4ReceiveEvent::TYPE)) {
            auto *ip4Datagram = ((IP4ReceiveEvent &) event).getDatagram();
            switch (ip4Datagram->getIP4ProtocolType()) {
                case IP4DataPart::IP4ProtocolType::ICMP4:
                    eventBus->publish(
                            new Kernel::ICMP4ReceiveEvent(
                                    (ICMP4Message *) ip4Datagram->getIP4DataPart()
                            )
                    );
                    return;
                case IP4DataPart::IP4ProtocolType::UDP:
                    eventBus->publish(
                            new Kernel::UDPReceiveEvent(
                                    (UDPDatagram *) ip4Datagram->getIP4DataPart()
                            )
                    );
                    return;
                default:
                    log.info("IP4ProtocolType of incoming IP4Datagram not supported, discarding");
                    delete ip4Datagram;
                    return;
            }
        }
        if ((event.getType() == ARPReceiveEvent::TYPE)) {
            auto *arpMessage = ((ARPReceiveEvent &) event).getARPMessage();
            //TODO: Implement processing of incoming ARP Messages here
            switch (arpMessage->getOpCode()) {
                case ARPMessage::OpCode::REQUEST:
                    break;
                case ARPMessage::OpCode::REPLY:
                    break;
                default: {
                    log.info("IP4ProtocolType of incoming IP4Datagram not supported, discarding");
                    delete arpMessage;
                    return;
                }
            }
            return;
        }
    }
}
