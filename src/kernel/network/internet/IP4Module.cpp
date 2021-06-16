//
// Created by hannes on 14.05.21.
//

#include <kernel/network/udp/UDP4Datagram.h>
#include <kernel/event/network/UDP4ReceiveEvent.h>
#include "IP4Module.h"

namespace Kernel {
    IP4Module::IP4Module(NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        this->routingModule = new IP4RoutingModule();
        this->interfaces = new Util::HashMap<EthernetDevice *, IP4Interface *>();
    }

    IP4Module::~IP4Module() {
        //No 'delete interfaces' necessary here, HashMaps are deleted automatically
        delete routingModule;
    }

    void IP4Module::collectIP4InterfaceAttributes(Util::ArrayList<String> *strings) {
        if (strings == nullptr || interfaces == nullptr) {
            return;
        }
        for (EthernetDevice *currentDevice:interfaces->keySet()) {
            strings->add(interfaces->get(currentDevice)->asString());
        }
    }

    void IP4Module::collectIP4RouteAttributes(Util::ArrayList<String> *strings) {
        if (strings == nullptr || routingModule == nullptr) {
            return;
        }
        routingModule->collectIP4RouteAttributes(strings);
    }

    void IP4Module::registerDevice(EthernetDevice *device, IP4Address *ip4Address, IP4Netmask *ip4Netmask) {
        if (device == nullptr || ip4Address == nullptr || ip4Netmask == nullptr) {
            log.error("At least one given parameter was null, not registering new device");
            return;
        }
        if (interfaces == nullptr || routingModule == nullptr) {
            log.error("Internal interface list or routing module was null, not registering new device");
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
            log.error("Given device was null, not unregistering device");
            return;
        }
        if (interfaces == nullptr || routingModule == nullptr) {
            log.error("Internal interface list or routing module was null, not unregistering device");
            return;
        }
        if (interfaces->containsKey(device)) {
            routingModule->removeRoutesFor(interfaces->get(device));
            interfaces->remove(device);
        }
    }

    void IP4Module::onEvent(const Event &event) {
        if ((event.getType() == IP4SendEvent::TYPE)) {
            IP4Datagram *datagram = ((IP4SendEvent &) event).getDatagram();
            if (routingModule == nullptr) {
                log.error("Internal routing module was null, not sending anything");
                //delete on NULL objects simply does nothing!
                delete datagram;
                return;
            }
            if (datagram == nullptr) {
                log.error("Outgoing datagram was null, ignoring");
                return;
            }
            if (datagram->getLengthInBytes() == 0) {
                log.error("Outgoing datagram was empty, discarding it");
                delete datagram;
                return;
            }
            switch (routingModule->sendViaBestRoute(datagram)) {
                case 0:
                    //Datagram will be deleted in EthernetModule after send
                    //-> no delete here!
                    return;
                case IP4_DATAGRAM_NULL: {
                    log.error("Outgoing datagram was null, ignoring");
                    return;
                }
                case IP4_NO_ROUTE_FOUND: {
                    log.error("No route to host could be found, discarding datagram");
                    break;
                }
                case IP4_INTERFACE_NULL: {
                    log.error("Outgoing interface was null, discarding datagram");
                    break;
                }
                case IP4_RECEIVER_ADDRESS_NULL: {
                    log.error("Given receiver address was null, discarding datagram");
                    break;
                }
                case IP4_MATCHING_BITS_FUNCTION_BROKEN: {
                    log.error("matchingBits() function in routing module is broken, discarding datagram");
                    break;
                }
                default: {
                    log.error("Sending failed, discarding datagram. See syslog for more details");
                    break;
                }
            }
            //Cleanup after logging errors
            delete datagram;
            return;
        }

        if ((event.getType() == IP4ReceiveEvent::TYPE)) {
            auto *ip4Header = ((IP4ReceiveEvent &) event).getHeader();
            auto *input = ((IP4ReceiveEvent &) event).getInput();

            if (ip4Header == nullptr) {
                log.error("Incoming IP4Datagram was null, discarding input");
                delete input;
                return;
            }
            if (input == nullptr) {
                log.error("Incoming input was null, discarding datagram");
                delete ip4Header;
                return;
            }
            if(!ip4Header->headerValid()){
                log.error("Incoming IP4Header corrupted, discarding datagram");
                delete ip4Header;
                delete input;
                return;
            }
            switch (ip4Header->getIP4ProtocolType()) {
                case IP4DataPart::IP4ProtocolType::ICMP4: {
                    if (input->bytesRemaining() == 0) {
                        log.error("Incoming ICMP4Message was empty, discarding");
                        delete ip4Header;
                        delete input;
                        return;
                    }
                    //We don't care about all the possible ICMP4 messages here
                    //-> send full input to ICMP4Module for parsing and processing
                    eventBus->publish(new ICMP4ReceiveEvent(ip4Header, input));

                    //We need input AND ip4datagram in next module
                    //-> don't delete anything here!
                    return;
                }
                case IP4DataPart::IP4ProtocolType::UDP: {
                    auto *udp4Header = new UDP4Header();
                    if (udp4Header->parse(input)) {
                        log.error("Could not assemble UDP header, discarding data");
                        //udpDatagram is not part of ip4Datagram here
                        //-> we need to delete it separately!
                        delete udp4Header;
                        delete ip4Header;
                        delete input;
                        return;
                    }
                    eventBus->publish(new UDP4ReceiveEvent(udp4Header, ip4Header, input));
                    //We need input AND ip4datagram in next module
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
            auto *arpMessage = ((ARPReceiveEvent &) event).getARPMessage();
            EthernetDataPart::EtherType arpProtocolType =
                    EthernetDataPart::parseIntAsEtherType(
                            arpMessage->getProtocolType()
                    );
            if (arpProtocolType != EthernetDataPart::EtherType::IP4) {
                log.error("Incoming ARP message was not for IPv4, discarding");
                delete arpMessage;
                return;
            }
            if(interfaces== nullptr){
                log.error("Internal interface list not initialized, discarding");
                delete arpMessage;
                return;
            }
            auto *destinationAddress =
                    new IP4Address(arpMessage->getTargetProtocolAddress());
            IP4Interface *currentInterface;
            for (EthernetDevice *currentDevice:interfaces->keySet()) {
                currentInterface = interfaces->get(currentDevice);
                if (currentInterface->getIp4Address()->equals(destinationAddress)) {
                    if (currentInterface->notifyARPModule(arpMessage)) {
                        log.error("Notify ARP module failed");
                    }
                    //All data processed, final cleanup
                    delete arpMessage;
                    return;
                }
            }
            log.error("No matching interface for ARP message found, discarding");
            delete arpMessage;
            return;
        }
    }
}
