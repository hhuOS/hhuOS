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
#include "UDP4Port.h"

namespace Kernel {

    class UDP4SocketController {
    private:
        NetworkByteBlock *receiveBuffer = nullptr;
        NetworkEventBus *eventBus = nullptr;

    public:
        explicit UDP4SocketController(NetworkEventBus *eventBus, size_t bufferSize);

        uint8_t notifySocket(NetworkByteBlock *input);

        int receive(uint8_t* targetBuffer, size_t length);

        uint8_t publishSendEvent(IP4Address *destinationAddress, UDP4Datagram *outDatagram);
    };
}

#endif //HHUOS_UDP4SOCKETCONTROLLER_H
