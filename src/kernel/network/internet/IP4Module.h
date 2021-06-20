//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4MODULE_H
#define HHUOS_IP4MODULE_H

#include <kernel/network/NetworkEventBus.h>
#include <kernel/network/internet/routing/IP4RoutingModule.h>
#include <kernel/network/ethernet/EthernetDevice.h>

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

        uint8_t registerDevice(EthernetDevice *device, IP4Address *ip4Address, IP4Netmask *ip4Netmask);

        uint8_t unregisterDevice(EthernetDevice *device);

        void collectIP4InterfaceAttributes(Util::ArrayList<String> *strings);

        void collectIP4RouteAttributes(Util::ArrayList<String> *strings);
    };
}


#endif //HHUOS_IP4MODULE_H
