//
// Created by hannes on 14.06.21.
//

#ifndef HHUOS_UDP4SOCKETCONTROLLER_H
#define HHUOS_UDP4SOCKETCONTROLLER_H

#include <kernel/network/udp/UDP4Header.h>
#include <kernel/log/Logger.h>
#include <kernel/network/udp/UDP4Datagram.h>
#include "UDP4InputEntry.h"

namespace Kernel {
    class UDP4SocketController {
    private:
        EventBus *eventBus = nullptr;
        Spinlock *accessLock = nullptr;
        Atomic<bool> *isClosed = nullptr;

        Util::RingBuffer<UDP4InputEntry *> *inputBuffer = nullptr;

        Logger &log = Logger::get("UDP4SocketController");

        static void yield();

    public:

        explicit UDP4SocketController(EventBus *eventBus, uint16_t bufferSize);

        virtual ~UDP4SocketController();

        uint8_t shutdown();

        uint8_t startup();

        uint8_t notify(IP4Header *incomingIP4Header, UDP4Header *incomingUDP4Header, NetworkByteBlock *incomingInput);

        uint8_t receive(uint16_t *totalBytesRead, void *targetBuffer, uint16_t length, IP4Header **ip4HeaderVariable,
                        UDP4Header **udp4HeaderVariable);

        uint8_t publishSendEvent(
                IP4Address *destinationAddress, uint16_t sourcePort, uint16_t destinationPort,
                NetworkByteBlock *outData);
    };
}

#endif //HHUOS_UDP4SOCKETCONTROLLER_H
