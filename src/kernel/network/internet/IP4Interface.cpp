//
// Created by hannes on 25.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/network/NetworkDefinitions.h>
#include <kernel/service/TimeService.h>
#include "IP4Interface.h"

namespace Kernel {
    IP4Interface::IP4Interface(Kernel::NetworkEventBus *eventBus, EthernetDevice *ethernetDevice,
                               IP4Address *ip4Address,
                               IP4Netmask *ip4Netmask) {
        this->eventBus = eventBus;
        arpModule = new ARPModule(eventBus, nullptr);
        this->ethernetDevice = ethernetDevice;
        this->ip4Address = ip4Address;
        this->ip4Netmask = ip4Netmask;
        if (ip4Netmask->extractNetPart(&ip4NetAddress, ip4Address)) {
            delete ip4NetAddress;
            ip4NetAddress = nullptr;
        }

        //TODO: Reactivate this one when finished with ARP!
//        arpModule->addEntry(ip4Address, ethernetDevice->getAddress());
    }

    IP4Interface::~IP4Interface() {
        delete arpModule;
        delete ip4Address;
        delete ip4Netmask;
        delete ip4NetAddress;
        //ethernetDevices are stored in a separate module
        //no delete here!
    }

    uint8_t IP4Interface::sendIP4Datagram(IP4Address *targetProtocolAddress, IP4Datagram *ip4Datagram) {
        if (ethernetDevice == nullptr) {
            log.error("Connected EthernetDevice was null, not sending anything");
            return 1;
        }
        if (ip4Datagram == nullptr) {
            log.error("%s: Given IP4 datagram was null, return",
                      ethernetDevice->getIdentifier()->getCharacters()
            );
            return 1;
        }
        if (targetProtocolAddress == nullptr) {
            log.error("%s: Given receiver IP4 address was null, return",
                      ethernetDevice->getIdentifier()->getCharacters()
            );
            return 1;
        }
        if (arpModule == nullptr) {
            log.error("%s: ARP module was not initialized, do not send anything",
                      ethernetDevice->getIdentifier()->getCharacters()
            );
            return 1;
        }
        //We need to copy our own address, because the datagram's address will be deleted after sending
        ip4Datagram->setSourceAddress(new IP4Address(ip4Address));

        EthernetAddress *targetHardwareAddress = nullptr;
        if (arpModule->resolveTo(&targetHardwareAddress, targetProtocolAddress, this->ip4Address)) {
            log.error("%s: ARP module failed to resolve destination address, do not send anything",
                      ethernetDevice->getIdentifier()->getCharacters()
            );
            return 1;
        }
        eventBus->publish(
                new Kernel::EthernetSendEvent(
                        ethernetDevice,
                        //The frame's attributes will be deleted after sending
                        //-> copy it here!
                        new EthernetFrame(new EthernetAddress(targetHardwareAddress),
                                          ip4Datagram)
                )
        );
        return 0;
    }

    IP4Address *IP4Interface::getIp4Address() const {
        return ip4Address;
    }

    IP4Netmask *IP4Interface::getIp4Netmask() const {
        return ip4Netmask;
    }

    IP4Address *IP4Interface::getNetAddress() const {
        return ip4NetAddress;
    }

    bool IP4Interface::equals(IP4Interface *compare) {
        if (ethernetDevice == nullptr || compare == nullptr) {
            return false;
        }
        return ethernetDevice == compare->ethernetDevice;
    }

    String IP4Interface::asString() {
        if (ethernetDevice == nullptr || ip4Netmask == nullptr || ip4Address == nullptr) {
            return "NULL";
        }
        return ethernetDevice->asString() + ",\nIP4Address: " + ip4Address->asString() + ",\nIP4Netmask: " +
               ip4Netmask->asString();
    }

    uint8_t IP4Interface::notifyARPModule(ARPMessage *message) {
        if (ethernetDevice == nullptr || eventBus == nullptr) {
            return 1;
        }
        if (message == nullptr) {
            log.error("%s: ARP message was null, not processing",
                      ethernetDevice->getIdentifier()
            );
            return 1;
        }
        if (arpModule == nullptr) {
            log.error("%s: ARP module was not initialized, not processing ARP message",
                      ethernetDevice->getIdentifier()
            );
            return 1;
        }
        switch (message->getOpCode()) {
            case ARPMessage::OpCode::REQUEST: {
                //Use each message as a possible ARP update
                //TODO: Synchronize access!!
                arpModule->addEntry(
                        new IP4Address(message->getSenderProtocolAddress()),
                        new EthernetAddress(message->getSenderHardwareAddress())
                );

                uint8_t myAddressAsBytes[MAC_SIZE];
                ethernetDevice->getAddress()->copyTo(myAddressAsBytes);

                auto *response = message->buildResponse(myAddressAsBytes);
                auto *outFrame =
                        new EthernetFrame(new EthernetAddress(myAddressAsBytes), response);
                eventBus->publish(
                        new Kernel::EthernetSendEvent(ethernetDevice, outFrame)
                );
                break;
            }
            case ARPMessage::OpCode::REPLY: {
                //TODO: Synchronize access!!
                arpModule->addEntry(
                        new IP4Address(message->getSenderProtocolAddress()),
                        new EthernetAddress(message->getSenderHardwareAddress())
                );
                break;
            }
            case ARPMessage::OpCode::INVALID: {
                return 1;
            }
        }
        return 0;
    }
}