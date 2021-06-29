//
// Created by hannes on 17.06.21.
//

#ifndef HHUOS_UDP4SERVERSOCKET_H
#define HHUOS_UDP4SERVERSOCKET_H

#include <kernel/service/NetworkService.h>
#include <kernel/service/TimeService.h>

namespace Kernel {
    class UDP4ServerSocket {
    private:
        uint16_t listeningPort = 0;
        NetworkService *networkService = nullptr;
        IP4Address *destinationAddress = nullptr;
        UDP4SocketController *controller = nullptr;

    public:
        explicit UDP4ServerSocket(uint16_t listeningPort);

        virtual ~UDP4ServerSocket();

        uint8_t bind();

        uint8_t close();

        uint8_t send(IP4Address *givenDestination, uint16_t givenRemotePort, void *dataBytes, size_t length);

        uint8_t receive(void *targetBuffer, size_t length);

        uint8_t receive(size_t *totalBytesRead, void *targetBuffer, size_t length);

        uint8_t receive(size_t *totalBytesRead, void *targetBuffer, size_t length, IP4Header **ip4HeaderVariable,
                        UDP4Header **udp4HeaderVariable);
    };
}

#endif //HHUOS_UDP4SERVERSOCKET_H
