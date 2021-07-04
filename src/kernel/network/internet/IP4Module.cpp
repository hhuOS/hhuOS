//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/UDP4ReceiveEvent.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include "IP4Module.h"

namespace Kernel {
    //Private method!
    bool IP4Module::isForUsOrBroadcast(IP4Header *ip4Header) {
        if (ip4Header->destinationIs(broadcastAddress)) {
            return true;
        }
        if (interfaces == nullptr || accessLock == nullptr) {
            log.error("Internal interface list or accessLock was null, not checking if datagram is for us");
            return false;
        }
        accessLock->acquire();
        for (IP4Interface *current:*interfaces) {
            if (current->isDestinationOf(ip4Header)) {
                accessLock->release();
                return true;
            }
        }
        accessLock->release();
        return false;
    }

    //Private method!
    uint8_t IP4Module::notifyDestinationInterface(ARPMessage *arpMessage) {
        if (interfaces == nullptr || accessLock == nullptr) {
            log.error("Internal interface list or accessLock not initialized, discarding ARP message");
            return 1;
        }
        EthernetDataPart::EtherType arpProtocolType =
                EthernetDataPart::parseIntAsEtherType(arpMessage->getProtocolType());
        if (arpProtocolType != EthernetDataPart::EtherType::IP4) {
            log.error("Incoming ARP message was not for IPv4, discarding ARP message");
            return 1;
        }

        //We already checked above if this ARPMessage is for IPv4 or not
        //-> parsing works here
        auto *destinationAddress =
                new IP4Address(arpMessage->getTargetProtocolAddress());
        accessLock->acquire();
        IP4Interface *targetInterface = nullptr;
        for (IP4Interface *current:*interfaces) {
            if (current->hasAddress(destinationAddress)) {
                targetInterface = current;
                break;
            }
        }
        if (targetInterface == nullptr) {
            log.error("No target interface found for address %s, discarding ARP message",
                      destinationAddress->asChars());
            accessLock->release();
            delete destinationAddress;
            return 1;
        }
        delete destinationAddress;

        if (targetInterface->notify(arpMessage)) {
            log.error("Notify interface about ARP message failed, see syslog for more details");
            accessLock->release();
            return 1;
        }
        accessLock->release();
        return 0;
    }

    IP4Module::IP4Module(EventBus *eventBus) {
        this->eventBus = eventBus;
        routingModule = new IP4RoutingModule();
        broadcastAddress = IP4Address::buildBroadcastAddress();
        interfaces = new Util::ArrayList<IP4Interface *>();
        accessLock = new Spinlock();
        accessLock->release();
    }

    IP4Module::~IP4Module() {
        delete accessLock;
        if (interfaces == nullptr || routingModule == nullptr) {
            return;
        }
        IP4Interface *toDelete;
        for (size_t i = 0; i < interfaces->size(); i++) {
            //Deleting while iterating is always dangerous
            //-> execute get() and remove() separately!
            toDelete = interfaces->get(i);
            routingModule->removeRoutesFor(toDelete);
            interfaces->remove(i);
            i--;
            delete toDelete;
        }
        delete interfaces;
        delete routingModule;
    }

    uint8_t IP4Module::collectIP4InterfaceAttributes(Util::ArrayList<String> *strings) {
        if (strings == nullptr || interfaces == nullptr || accessLock == nullptr) {
            log.error("Given Strings list, internal interface list or accessLock was null, "
                      "not unregistering device");
            return 1;
        }
        accessLock->acquire();
        for (IP4Interface *current:*interfaces) {
            strings->add(current->asString());
        }
        accessLock->release();
        return 0;
    }

    uint8_t IP4Module::collectIP4RouteAttributes(Util::ArrayList<String> *strings) {
        if (strings == nullptr || routingModule == nullptr) {
            log.error("Given String list or routing module was null, not collecting route attributes");
            return 1;
        }
        return routingModule->collectIP4RouteAttributes(strings);
    }

    uint8_t IP4Module::registerDevice(EthernetDevice *ethernetDevice, IP4Address *ip4Address, IP4Netmask *ip4Netmask) {
        if (ethernetDevice == nullptr || ip4Address == nullptr || ip4Netmask == nullptr) {
            log.error("At least one given parameter was null, not registering new device");
            return 1;
        }
        if (interfaces == nullptr || routingModule == nullptr || accessLock == nullptr) {
            log.error("Internal interface list, routing module or accessLock was null, not registering new device");
            return 1;
        }
        accessLock->acquire();
        for (IP4Interface *current:*interfaces) {
            if (current->connectedTo(ethernetDevice)) {
                log.error("Ethernet device %s already registered, not registering it again",
                          ethernetDevice->getIdentifier()->getCharacters());
                accessLock->release();
                return 1;
            }
        }

        auto *newInterface = new IP4Interface(eventBus, ethernetDevice, ip4Address, ip4Netmask);
        IP4Address *netAddress = nullptr;
        ip4Netmask->extractNetPart(&netAddress, ip4Address);

        if (routingModule->addDirectRouteFor(netAddress, ip4Netmask, newInterface)) {
            log.error("Adding route for new IP4Interface failed, rollback");
            accessLock->release();
            delete newInterface;
            return 1;
        }
        interfaces->add(newInterface);
        accessLock->release();
        return 0;
    }

    uint8_t IP4Module::unregisterDevice(EthernetDevice *ethernetDevice) {
        if (ethernetDevice == nullptr) {
            log.error("Given device was null, not unregistering device");
            return 1;
        }
        if (interfaces == nullptr || routingModule == nullptr || accessLock == nullptr) {
            log.error("Internal interface list, routing module or accessLock was null, not unregistering device");
            return 1;
        }
        accessLock->acquire();
        IP4Interface *toDelete;
        for (size_t i = 0; i < interfaces->size(); i++) {
            if (interfaces->get(i)->connectedTo(ethernetDevice)) {
                toDelete = interfaces->get(i);
                routingModule->removeRoutesFor(toDelete);
                interfaces->remove(i);
                accessLock->release();
                delete toDelete;
                return 0;
            }
        }

        accessLock->release();
        log.error("Given ethernet device was not registered, ignoring");
        return 1;
    }

    uint8_t IP4Module::setDefaultRoute(IP4Address *gatewayAddress, EthernetDeviceIdentifier *outDevice) {
        if (gatewayAddress == nullptr || outDevice == nullptr) {
            log.error("Gateway address or out device was null, not setting default route");
            return 1;
        }
        if (interfaces == nullptr || routingModule == nullptr || accessLock == nullptr) {
            log.error("Internal interface list, routing module or accessLock was null, not setting default route");
            return 1;
        }
        accessLock->acquire();
        for (IP4Interface *current:*interfaces) {
            if (current->connectedTo(outDevice)) {
                if (routingModule->setDefaultRoute(gatewayAddress, current)) {
                    accessLock->release();
                    log.error("Could not set default route");
                    return 1;
                }
                accessLock->release();
                return 0;
            }
        }
        accessLock->release();
        log.error("No device with identifier %s could be found, not setting default route",
                  (char *) outDevice->asString());
        return 1;
    }

    uint8_t IP4Module::removeDefaultRoute() {
        if (routingModule == nullptr) {
            log.error("Routing module was null, not setting default route");
            return 1;
        }
        return routingModule->removeDefaultRoute();
    }

    void IP4Module::onEvent(const Event &event) {
        if ((event.getType() == IP4SendEvent::TYPE)) {
            auto *destinationAddress = ((IP4SendEvent &) event).getDestinationAddress();
            auto *dataPart = ((IP4SendEvent &) event).getDataPart();

            if (dataPart == nullptr) {
                log.error("Outgoing data was null, ignoring");
                delete destinationAddress;
                return;
            }
            if (destinationAddress == nullptr) {
                log.error("Destination address was null, discarding outgoing data");
                destinationAddress = new IP4Address(0, 0, 0, 0);
                //IP4Datagram can cleanup IP4DataParts internally
                //-> build one with given data and delete it then
                auto *cleanup = new IP4Datagram(destinationAddress, dataPart);
                //destinationAddress will be deleted internally
                delete cleanup;
                return;
            }
            auto *datagram = new IP4Datagram(destinationAddress, dataPart);
            if (routingModule == nullptr) {
                log.error("Internal routing module was null, not sending anything");
                //destinationAddress will be deleted internally
                delete datagram;
                return;
            }
            if (datagram->getLengthInBytes() == 0) {
                log.error("Outgoing datagram was empty, discarding it");
                //destinationAddress will be deleted internally
                delete datagram;
                return;
            }
            if (routingModule->sendViaBestRoute(datagram)) {
                log.error("Sending failed, see syslog for more details");
                //destinationAddress will be deleted internally
                delete datagram;
            }
            //Datagram will be deleted in EthernetModule after sending
            //-> no delete here!
            return;
        }
        if ((event.getType() == IP4ReceiveEvent::TYPE)) {
            auto *input = ((IP4ReceiveEvent &) event).getInput();
            if (input == nullptr) {
                log.error("Incoming input was null, ignoring");
                return;
            }
            auto *ip4Header = new IP4Header();
            if (ip4Header->parse(input)) {
                log.error("Parsing IP4Header failed, discarding");
                delete ip4Header;
                delete input;
                return;
            }
            if (!ip4Header->headerIsValid()) {
                log.error("Incoming IP4Header corrupted, discarding datagram");
                delete ip4Header;
                delete input;
                return;
            }
            if(!isForUsOrBroadcast(ip4Header)){
                log.error("Incoming datagram is not for us and not broadcast either, discarding");
                delete ip4Header;
                delete input;
                return;
            }
            switch (ip4Header->getIP4ProtocolType()) {
                case IP4DataPart::IP4ProtocolType::ICMP4: {
                    //send input to ICMP4Module via EventBus for further processing
                    auto icmp4ReceiveInputEvent =
                            Util::SmartPointer<Event>(new ICMP4ReceiveEvent(ip4Header, input));
                    eventBus->publish(icmp4ReceiveInputEvent);

                    //We need input AND ip4Header in next module
                    //-> don't delete anything here!
                    return;
                }
                case IP4DataPart::IP4ProtocolType::UDP: {
                    //send input to UDP4Module via EventBus for further processing
                    auto udp4ReceiveInputEvent =
                            Util::SmartPointer<Event>(new UDP4ReceiveEvent(ip4Header, input));
                    eventBus->publish(udp4ReceiveInputEvent);

                    //We need input AND ip4Header in next module
                    //-> don't delete anything here!
                    return;
                }
                default:
                    log.error("IP4ProtocolType of incoming IP4Datagram not supported, discarding");
                    delete ip4Header;
                    delete input;
                    return;
            }
        }
        if ((event.getType() == ARPReceiveEvent::TYPE)) {
            //Each IP4 interface has its own ARP module (RFC1180 page 12 middle)
            //-> we need to find the destination interface first when processing ARP messages!
            auto *input = ((ARPReceiveEvent &) event).getInput();
            if (input == nullptr) {
                log.error("Incoming input was null, ignoring");
                return;
            }
            auto *arpMessage = new ARPMessage();
            if (arpMessage->parse(input)) {
                log.error("Could not assemble ARP message, discarding data");
                delete arpMessage;
                delete input;
                return;
            }
            if (notifyDestinationInterface(arpMessage)) {
                log.error("Could not notify destination interface, see syslog for more details");
            }
            //Processing finally done, cleanup
            delete arpMessage;
            delete input;
            return;
        }
    }
}
