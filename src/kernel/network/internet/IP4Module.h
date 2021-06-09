//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4MODULE_H
#define HHUOS_IP4MODULE_H

#include <kernel/log/Logger.h>
#include <kernel/core/System.h>

#include <kernel/event/Receiver.h>
#include <kernel/event/network/IP4SendEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include <kernel/event/network/UDP4ReceiveEvent.h>

#include <kernel/network/NetworkEventBus.h>

#include <kernel/network/arp/ARPModule.h>
#include <kernel/network/arp/ARPMessage.h>

#include <kernel/network/internet/routing/IP4RoutingModule.h>

#include <kernel/network/internet/icmp/messages/ICMP4EchoReply.h>
#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>

#include "IP4Interface.h"
#include "IP4Module.h"

#define IP4_DELIVER_SUCCESS 0
#define IP4_DATAGRAM_NULL 1
#define IP4_INTERFACE_NULL 2
#define IP4_RECEIVER_ADDRESS_NULL 3
#define IP4_MATCHING_BITS_FUNCTION_BROKEN 4
#define IP4_NO_ROUTE_FOUND 5

namespace Kernel {

    class IP4Module final : public Receiver {
    private:
        NetworkEventBus *eventBus = nullptr;
        IP4RoutingModule *routingModule = nullptr;
        Util::HashMap<EthernetDevice *, IP4Interface *> *interfaces = nullptr;

    public:

        explicit IP4Module(NetworkEventBus *eventBus);

        ~IP4Module() override;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("IP4Module");


/**
     * Inherited method from Receiver.
     * This method is meant to be overridden and
     * implemented by this class.
     */
        void onEvent(const Event &event) override;

        void registerDevice(EthernetDevice *device, IP4Address *ip4Address, IP4Netmask *ip4Netmask);

        void unregisterDevice(EthernetDevice *device);

        void collectIP4InterfaceAttributes(Util::ArrayList<String> *strings);

        void collectIP4RouteAttributes(Util::ArrayList<String> *strings);
    };

}


#endif //HHUOS_IP4MODULE_H
