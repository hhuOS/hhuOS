//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDP4MODULE_H
#define HHUOS_UDP4MODULE_H


#include <kernel/network/udp/sockets/UDP4SocketController.h>
#include <kernel/network/NetworkDefinitions.h>

namespace Kernel {
    class UDP4Module : public Receiver {
    private:
        EventBus *eventBus = nullptr;
        Util::HashMap<uint16_t, UDP4SocketController *> *sockets = nullptr;
        Spinlock *accessLock = nullptr;

        uint8_t notifyDestinationSocket(UDP4Header *udp4Header, IP4Header *ip4Header, NetworkByteBlock *input);

    public:

        explicit UDP4Module(EventBus *eventBus);

        Logger &log = Logger::get("UDP4Module");

        /**
         * Inherited method from Receiver.
         * This method is meant to be overridden and
         * implemented by this class.
         */
        void onEvent(const Event &event) override;

        uint8_t registerControllerFor(uint16_t destinationPort, UDP4SocketController *controller);

        uint8_t registerControllerFor(uint16_t *destinationPortTarget, UDP4SocketController *controller);

        uint8_t unregisterControllerFor(uint16_t destinationPort);

        ~UDP4Module() override;
    };

}

#endif //HHUOS_UDP4MODULE_H
