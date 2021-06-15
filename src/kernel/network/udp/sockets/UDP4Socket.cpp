//
// Created by hannes on 13.06.21.
//

#include "UDP4Socket.h"

namespace Kernel {
    UDP4Socket::UDP4Socket(uint16_t listeningPort) {
        this->listeningPort = new UDP4Port(listeningPort);
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController();
    }

    UDP4Socket::UDP4Socket(IP4Address *targetAddress, uint16_t remotePort) {
        this->destinationAddress = targetAddress;
        this->remotePort = new UDP4Port(remotePort);
        this->listeningPort = new UDP4Port(16123);
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController();
    }

    UDP4Socket::~UDP4Socket() {
        close();
        delete listeningPort;
        delete remotePort;
        delete controller;
    }

    uint8_t UDP4Socket::bind() {
        if(networkService->registerSocketController(listeningPort,controller)){
            return 1;
        }
        return 0;
    }

    uint8_t UDP4Socket::close() {
        return networkService->unregisterSocketController(listeningPort);
    }

    uint8_t UDP4Socket::send(char *dataBytes, size_t length) {
        if (
                dataBytes == nullptr ||
                destinationAddress == nullptr ||
                length == 0
                ) {
            return 1;
        }
        //Trivial but helpful to centralize access here
        //-> we can filter, convert etc. incoming data here if necessary
        return send((uint8_t *) dataBytes, length);
    }

    uint8_t UDP4Socket::send(uint8_t *dataBytes, size_t length) {
        if (
                dataBytes == nullptr ||
                destinationAddress == nullptr ||
                length == 0
                ) {
            return 1;
        }
        controller->publishSendEvent(
                destinationAddress,
                             new UDP4Datagram(listeningPort, remotePort, dataBytes, length)
        );
        return 0;
    }

    int UDP4Socket::receive(uint8_t *targetBuffer, size_t length) {
        return controller->receive(targetBuffer, length);
    }
}