//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_UDP4CLIENTSOCKET_H
#define HHUOS_UDP4CLIENTSOCKET_H

#include <kernel/service/TimeService.h>
#include <kernel/service/NetworkService.h>

namespace Kernel {
    class UDP4ClientSocket {
    private:
        uint16_t listeningPort = 0, targetPort = 0;
        TimeService *timeService= nullptr;
        NetworkService *networkService = nullptr;
        IP4Address *destinationAddress = nullptr;
        UDP4SocketController *controller = nullptr;

    public:
        UDP4ClientSocket(IP4Address *targetAddress, uint16_t targetPort);

        virtual ~UDP4ClientSocket();

        uint8_t bind();

        uint8_t close();

        uint8_t send(void *dataBytes, size_t length);

        uint8_t receive(void *targetBuffer, size_t length);

        uint8_t receive(size_t *totalBytesRead, void *targetBuffer, size_t length);
    };
}

#endif //HHUOS_UDP4CLIENTSOCKET_H
