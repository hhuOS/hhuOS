//
// Created by hannes on 14.06.21.
//

#ifndef HHUOS_UDP4SOCKETCONTROLLER_H
#define HHUOS_UDP4SOCKETCONTROLLER_H

#include <kernel/network/udp/UDP4Header.h>
#include <kernel/network/NetworkEventBus.h>
#include <kernel/log/Logger.h>
#include <kernel/network/udp/UDP4Datagram.h>

namespace Kernel {
    class UDP4SocketController {
    private:
        IP4Header *ip4Header = nullptr;
        UDP4Header *udp4Header = nullptr;
        NetworkByteBlock *content = nullptr;

        NetworkEventBus *eventBus = nullptr;
        Spinlock *readLock = nullptr, *writeLock = nullptr;
        Atomic<bool> *isClosed = nullptr;

        Logger &log = Logger::get("UDP4SocketController");

        void deleteData();
    public:
        explicit UDP4SocketController(NetworkEventBus *eventBus);

        uint8_t shutdown();

        uint8_t startup();

        uint8_t notifySocket(IP4Header *incomingIP4Header, UDP4Header *incomingUDP4Header, NetworkByteBlock *input);

        uint8_t receive(size_t *totalBytesRead, void *targetBuffer, size_t length, IP4Header **ip4HeaderVariable,
                        UDP4Header **udp4HeaderVariable);

        uint8_t publishSendEvent(IP4Address *destinationAddress, UDP4Datagram *outDatagram);
    };
}

#endif //HHUOS_UDP4SOCKETCONTROLLER_H
