//
// Created by hannes on 25.05.21.
//

#include <kernel/event/network/EthernetSendEvent.h>
#include "IP4Interface.h"

namespace Kernel {
    IP4Interface::IP4Interface(NetworkEventBus *eventBus, EthernetDevice *ethernetDevice,
                               IP4Address *ip4Address,
                               IP4Netmask *ip4Netmask) {
        this->eventBus = eventBus;
        //Addresses in ARPModule may be deleted at any time
        //-> copy our addresses or we might lose them!
        arpModule = new ARPModule(eventBus, ethernetDevice);
        this->ethernetDevice = ethernetDevice;
        this->ip4Address = ip4Address;
        this->ip4Netmask = ip4Netmask;
        if (ip4Netmask->extractNetPart(&ip4NetAddress, ip4Address)) {
            delete ip4NetAddress;
            ip4NetAddress = nullptr;
        }
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
            log.error("%s: ARP module was not initialized, return",
                      ethernetDevice->getIdentifier()->getCharacters()
            );
            return 1;
        }
        //We need to copy our own address, because the datagram's address will be deleted after sending
        ip4Datagram->setSourceAddress(new IP4Address(ip4Address));
        if (ip4Datagram->fillHeaderChecksum()) {
            log.error("Header checksum calculation failed!");
            return 1;
        }

        EthernetAddress *targetHardwareAddress = nullptr;
        if (arpModule->resolveTo(&targetHardwareAddress, targetProtocolAddress, ip4Address)) {
            log.error("%s: ARP module failed to resolve destination address",
                      ethernetDevice->getIdentifier()->getCharacters()
            );
            return 1;
        }

        //The frame's attributes will be deleted after sending
        //-> copy it here!
        auto *targetHardwareAddressCopy = new EthernetAddress(targetHardwareAddress);
        //Send data to EthernetModule via EventBus for further processing
        eventBus->publish(new EthernetSendEvent(ethernetDevice, targetHardwareAddressCopy, ip4Datagram));

        //Datagram will be deleted in EthernetModule after sending
        //-> no delete 'targetHardwareAddressCopy' here!
        return 0;
    }

    bool IP4Interface::equals(IP4Interface *compare) {
        if (ethernetDevice == nullptr || compare == nullptr) {
            return false;
        }
        return ethernetDevice == compare->ethernetDevice;
    }

    String IP4Interface::asString() {
        if (ethernetDevice == nullptr || ip4Netmask == nullptr || ip4Address == nullptr || arpModule == nullptr) {
            return "NULL";
        }
        return ethernetDevice->asString() + ",\nIP4Address: " + ip4Address->asString() + ",\nIP4Netmask: " +
               ip4Netmask->asString() + ",\nARPTable: {" + arpModule->asString() + "}";
    }

    uint8_t IP4Interface::notify(ARPMessage *arpMessage) {
        if (ethernetDevice == nullptr || eventBus == nullptr) {
            return 1;
        }
        if (arpMessage == nullptr) {
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
        return arpModule->processIncoming(arpMessage);
    }

    bool IP4Interface::connectedTo(EthernetDevice *otherDevice) {
        if (ethernetDevice == nullptr || otherDevice == nullptr) {
            return false;
        }
        return ethernetDevice == otherDevice;
    }

    bool IP4Interface::hasAddress(IP4Address *otherAddress) {
        if (this->ip4Address == nullptr || otherAddress == nullptr) {
            return false;
        }
        return this->ip4Address->equals(otherAddress);
    }
}