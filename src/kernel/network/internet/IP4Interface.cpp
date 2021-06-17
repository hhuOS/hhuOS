//
// Created by hannes on 25.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include "IP4Interface.h"

namespace Kernel {
    IP4Interface::IP4Interface(Kernel::NetworkEventBus *eventBus, EthernetDevice *ethernetDevice,
                               IP4Address *ip4Address,
                               IP4Netmask *ip4Netmask) {
        this->eventBus = eventBus;
        this->arpModule = new ARPModule();
        this->ethernetDevice = ethernetDevice;
        this->ip4Address = ip4Address;
        this->ip4Netmask = ip4Netmask;

        this->arpModule->addEntry(ip4Address, ethernetDevice->getAddress());
    }

    IP4Interface::~IP4Interface() {
        delete this->arpModule;
    }

    uint8_t IP4Interface::sendIP4Datagram(IP4Address *receiverAddress, IP4Datagram *ip4Datagram) {
        if (ip4Datagram == nullptr){
            log.error("%s: Given IP4 datagram was null, return", ethernetDevice->getIdentifier());
            return 1;
        }
        if(receiverAddress == nullptr) {
            log.error("%s: Given receiver IP4 address was null, return", ethernetDevice->getIdentifier());
            return 1;
        }
        //interface selection happens in routing module
        // -> we don't know source address before this point here!
        ip4Datagram->setSourceAddress(this->ip4Address);
        EthernetAddress *destinationAddress = nullptr;
        uint8_t arpError = arpModule->resolveTo(&destinationAddress, receiverAddress);
        if (arpError) {
            return arpError;
        }

        if (destinationAddress == nullptr) {
            //See RFC 826 page 3 for details
            auto *arpRequest = new ARPMessage(
                    1, // 1 for Ethernet
                    (uint16_t) EthernetDataPart::EtherType::IP4, // 0x0800 for IPv4
                    MAC_SIZE,
                    IP4ADDRESS_LENGTH,
                    ARPMessage::OpCode::REQUEST
            );

            uint8_t hardwareAddress[MAC_SIZE];
            uint8_t protocolAddress[IP4ADDRESS_LENGTH];

            this->ethernetDevice->getAddress()->copyTo(hardwareAddress);
            arpRequest->setSenderHardwareAddress(hardwareAddress);

            this->ip4Address->copyTo(protocolAddress);
            arpRequest->setSenderProtocolAddress(protocolAddress);

            arpModule->getBroadcastAddress()->copyTo(hardwareAddress);
            arpRequest->setTargetHardwareAddress(hardwareAddress);

            receiverAddress->copyTo(protocolAddress);
            arpRequest->setTargetProtocolAddress(protocolAddress);

            //TODO: Add Datagram to internal data structure for pending requests
            this->eventBus->publish(
                    new Kernel::EthernetSendEvent(
                            this->ethernetDevice,
                            new EthernetFrame(
                                    arpModule->getBroadcastAddress(), arpRequest
                            )
                    )
            );
            return 0;
        }
        this->eventBus->publish(
                new Kernel::EthernetSendEvent(
                        this->ethernetDevice,
                        new EthernetFrame(destinationAddress, ip4Datagram)
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

    bool IP4Interface::equals(IP4Interface *compare) {
        return this->ethernetDevice == compare->ethernetDevice;
    }

    IP4Address *IP4Interface::getNetAddress() const {
        return this->ip4Netmask->extractNetPart(this->getIp4Address());
    }

    String IP4Interface::asString() {
        return this->ethernetDevice->asString() + ",\nIP4Address: " + this->ip4Address->asString() + ",\nIP4Netmask: " +
               this->ip4Netmask->asString();
    }

    uint8_t IP4Interface::notifyARPModule(ARPMessage *message) {
        switch (message->getOpCode()) {
            case ARPMessage::OpCode::REQUEST: {
                //Use each message as a possible ARP update
                //TODO: Synchronize access!!
                arpModule->addEntry(
                        new IP4Address(message->getSenderProtocolAddress()),
                        new EthernetAddress(message->getSenderHardwareAddress())
                );

                uint8_t myAddressAsBytes[MAC_SIZE];
                this->ethernetDevice->getAddress()->copyTo(myAddressAsBytes);

                auto *response = message->buildResponse(myAddressAsBytes);
                auto *outFrame =
                        new EthernetFrame(new EthernetAddress(myAddressAsBytes), response);
                this->eventBus->publish(
                        new Kernel::EthernetSendEvent(this->ethernetDevice, outFrame)
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