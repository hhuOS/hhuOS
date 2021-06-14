//
// Created by hannes on 13.06.21.
//

#ifndef HHUOS_UDP4SOCKET_H
#define HHUOS_UDP4SOCKET_H

#define BUFFER_SIZE 1024

#include <cstdint>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/NetworkEventBus.h>
#include <kernel/service/NetworkService.h>

class UDP4Socket {
private:
    uint16_t listeningPort = 0, remotePort = 0;
    NetworkByteBlock *receiveBuffer = nullptr;
    IP4Address *destinationAddress = nullptr;
    Kernel::NetworkEventBus *eventBus = nullptr;
    Kernel::NetworkService *networkService = nullptr;

public:
    explicit UDP4Socket(uint16_t listeningPort);

    UDP4Socket(IP4Address *targetAddress, uint16_t remotePort);

    uint8_t bind();

    uint8_t close();

    uint8_t send(char *dataBytes, size_t length);

    uint8_t send(uint8_t *dataBytes, size_t length);

    int receive(uint8_t *targetBuffer);

    virtual ~UDP4Socket();
};


#endif //HHUOS_UDP4SOCKET_H
