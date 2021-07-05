//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_IP4INT_H
#define HHUOS_IP4INT_H


#include <kernel/network/arp/ARPMessage.h>
#include <kernel/network/arp/ARPModule.h>
#include <kernel/network/internet/IP4Netmask.h>
#include "IP4Datagram.h"

namespace Kernel {
    class IP4Interface {
    private:
        ARPModule *arpModule = nullptr;
        EventBus *eventBus = nullptr;
        IP4Address *ip4Address = nullptr, *ip4NetAddress = nullptr;
        IP4Netmask *ip4Netmask = nullptr;
        String ethernetIdentifier = "";
        EthernetDevice *ethernetDevice = nullptr;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("IP4Interface");

    public:

        IP4Interface(EventBus *eventBus, EthernetDevice *ethernetDevice, IP4Address *ip4Address,
                     IP4Netmask *ip4Netmask);

        virtual ~IP4Interface();

        bool equals(IP4Interface *compare);

        bool connectedTo(EthernetDevice *otherDevice);

        bool connectedTo(const String &identifier);

        bool hasAddress(IP4Address *otherAddress);

        bool isDestinationOf(IP4Header *ip4Header);

        uint8_t sendIP4Datagram(IP4Address *targetProtocolAddress, IP4Datagram *ip4Datagram);

        uint8_t notify(ARPMessage *arpMessage);

        String asString();

        String arpTableAsString();

        String ethernetDeviceAsString();
    };
}

#endif //HHUOS_IP4INT_H
