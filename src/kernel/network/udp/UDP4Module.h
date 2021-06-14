//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDP4MODULE_H
#define HHUOS_UDP4MODULE_H

#include <kernel/log/Logger.h>
#include <kernel/event/Event.h>
#include <kernel/event/Receiver.h>
#include <kernel/network/NetworkEventBus.h>
#include <kernel/network/udp/sockets/UDP4SocketManager.h>

namespace Kernel {

    class UDP4Module : public Receiver {
    private:
        NetworkEventBus *eventBus = nullptr;
        UDP4SocketManager *socketManager = nullptr;

    public:
        explicit UDP4Module(Kernel::NetworkEventBus *eventBus);

        Logger &log = Logger::get("UDP4Module");

        /**
         * Inherited method from Receiver.
         * This method is meant to be overridden and
         * implemented by this class.
         */
        void onEvent(const Event &event) override;

        uint8_t registerController(UDP4SocketController *attributes);

        uint8_t unregisterController(UDP4Port *port);
    };

}

#endif //HHUOS_UDP4MODULE_H
