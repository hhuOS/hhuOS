//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_UDP4SOCKET_H
#define HHUOS_UDP4SOCKET_H

#include <cstdint>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/NetworkEventBus.h>
#include <kernel/service/NetworkService.h>
#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/core/System.h>
#include <kernel/service/NetworkService.h>

namespace Kernel {
    class UDP4Socket {
    private:
        NetworkService *networkService = nullptr;
        UDP4Port *listeningPort = nullptr, *remotePort = nullptr;
        IP4Address *destinationAddress = nullptr;
        UDP4SocketController *controller = nullptr;

    public:
        explicit UDP4Socket(UDP4Port *listeningPort);

        UDP4Socket(IP4Address *targetAddress, UDP4Port *targetPort);

        virtual ~UDP4Socket();

        uint8_t bind();

        uint8_t close();

        uint8_t send(char *dataBytes, size_t length);

        uint8_t send(uint8_t *dataBytes, size_t length);

        int receive(uint8_t *targetBuffer, size_t length);

        void copyListeningPortTo(uint16_t* target);
    };
}

#endif //HHUOS_UDP4SOCKET_H