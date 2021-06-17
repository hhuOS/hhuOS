//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_UDP4CLIENTSOCKET_H
#define HHUOS_UDP4CLIENTSOCKET_H

#include <cstdint>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/NetworkEventBus.h>
#include <kernel/service/NetworkService.h>
#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/core/System.h>
#include <kernel/service/NetworkService.h>

namespace Kernel {
    class UDP4ClientSocket {
    private:
        uint8_t bindState = 1;
        uint16_t listeningPort = 0, targetPort = 0;
        NetworkService *networkService = nullptr;
        IP4Address *destinationAddress = nullptr;
        UDP4SocketController *controller = nullptr;

    public:
        UDP4ClientSocket(IP4Address *targetAddress, uint16_t targetPort);

        virtual ~UDP4ClientSocket();

        uint8_t close();

        uint8_t send(void *dataBytes, size_t length);

        int receive(void *targetBuffer, size_t length);
    };
}

#endif //HHUOS_UDP4CLIENTSOCKET_H
