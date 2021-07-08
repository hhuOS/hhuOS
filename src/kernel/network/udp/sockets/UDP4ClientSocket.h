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
        NetworkService *networkService = nullptr;
        IP4Address *destinationAddress = nullptr;
        UDP4SocketController *controller = nullptr;

    public:
        UDP4ClientSocket(IP4Address *targetAddress, uint16_t targetPort);

        virtual ~UDP4ClientSocket();

        uint8_t bind();

        uint8_t close();

        uint8_t send(void *dataBytes, uint16_t length);

        uint8_t receive(void *targetBuffer, uint16_t length);

        uint8_t receive(uint16_t *totalBytesRead, void *targetBuffer, uint16_t length);

        uint8_t receive(uint16_t *totalBytesRead, void *targetBuffer, uint16_t length, IP4Header **ip4HeaderVariable,
                        UDP4Header **udp4HeaderVariable);
    };
}

#endif //HHUOS_UDP4CLIENTSOCKET_H
