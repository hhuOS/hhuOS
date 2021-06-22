//
// Created by hannes on 25.05.21.
//

#ifndef HHUOS_IP4INT_H
#define HHUOS_IP4INT_H


#include <kernel/network/arp/ARPModule.h>
#include <kernel/network/NetworkEventBus.h>
#include <kernel/network/internet/addressing/IP4Netmask.h>
#include <kernel/network/ethernet/EthernetDevice.h>
#include <kernel/network/arp/ARPMessage.h>
#include "IP4Datagram.h"

namespace Kernel {
    class IP4Interface {
    private:
        ARPModule *arpModule = nullptr;
        NetworkEventBus *eventBus = nullptr;
        IP4Address *ip4Address = nullptr, *ip4NetAddress = nullptr;
        IP4Netmask *ip4Netmask = nullptr;
        EthernetDevice *ethernetDevice = nullptr;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("IP4Interface");

    public:

        IP4Interface(NetworkEventBus *eventBus, EthernetDevice *ethernetDevice, IP4Address *ip4Address,
                     IP4Netmask *ip4Netmask);

        uint8_t sendIP4Datagram(IP4Address *targetProtocolAddress, IP4Datagram *ip4Datagram);

        [[nodiscard]] IP4Address *getIp4Address() const;

        [[nodiscard]] IP4Netmask *getIp4Netmask() const;

        [[nodiscard]] IP4Address *getNetAddress() const;

        virtual ~IP4Interface();

        bool equals(IP4Interface *compare);

        String asString();

        uint8_t notifyARPModule(ARPMessage *message);
    };
}

#endif //HHUOS_IP4INT_H
