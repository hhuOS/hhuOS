//
// Created by hannes on 14.06.21.
//

#ifndef HHUOS_UDP4SOCKETCONTROLLER_H
#define HHUOS_UDP4SOCKETCONTROLLER_H

#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/udp/UDP4Datagram.h>
#include <kernel/network/NetworkEventBus.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/internet/IP4Header.h>

namespace Kernel {
    class UDP4SocketController {
    private:
        IP4Header *ip4Header = nullptr;
        UDP4Header *udp4Header = nullptr;
        NetworkByteBlock *content = nullptr;

        NetworkEventBus *eventBus = nullptr;
        Spinlock *readLock = nullptr, *writeLock = nullptr;

    public:
        explicit UDP4SocketController(NetworkEventBus *eventBus);

        uint8_t notifySocket(IP4Header *incomingIP4Header, UDP4Header *incomingUDP4Header, NetworkByteBlock *input);

        int receive(void *targetBuffer, size_t length, IP4Header **ip4HeaderVariable, UDP4Header **udp4HeaderVariable);

        uint8_t publishSendEvent(IP4Address *destinationAddress, UDP4Datagram *outDatagram);
    };
}

#endif //HHUOS_UDP4SOCKETCONTROLLER_H
