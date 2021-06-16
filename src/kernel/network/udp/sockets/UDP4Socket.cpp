//
// Created by hannes on 13.06.21.
//

#include "UDP4Socket.h"

namespace Kernel {
    UDP4Socket::UDP4Socket(UDP4Port *listeningPort) {
        this->listeningPort = new UDP4Port(listeningPort);
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController();
    }

    UDP4Socket::UDP4Socket(IP4Address *targetAddress, UDP4Port *targetPort) {
        this->destinationAddress = targetAddress;
        this->remotePort = new UDP4Port(targetPort);
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
        return networkService->registerSocketController(listeningPort, controller);
    }

    uint8_t UDP4Socket::close() {
        return networkService->unregisterSocketController(listeningPort);
    }

    //Client send()
    //-> destination address and remote port should be given via constructor
    uint8_t UDP4Socket::send(void *dataBytes, size_t length) {
        return send(this->destinationAddress, this->remotePort, dataBytes, length);
    }

    //Server send()
    //->we need to read destination address and remote port from incoming datagrams here
    uint8_t UDP4Socket::send(IP4Address *givenDestination, UDP4Port *givenRemotePort, void *dataBytes, size_t length) {
        if (
                dataBytes == nullptr ||
                givenDestination == nullptr ||
                givenRemotePort == nullptr ||
                length == 0
                ) {
            return 1;
        }
        controller->publishSendEvent(
                givenDestination,
                new UDP4Datagram(this->listeningPort, givenRemotePort, dataBytes, length)
        );
        return 0;
    }

    //Regular receive() for clients
    int UDP4Socket::receive(uint8_t *targetBuffer, size_t length) {
        return receive(targetBuffer, length, nullptr, nullptr);
    }

    //Extended receive() for server and clients who need to know IP4 or UDP4 headers
    int UDP4Socket::receive(uint8_t *targetBuffer, size_t length,IP4Header **ip4Header, UDP4Header **udp4Header) {
        return controller->receive(targetBuffer, length, ip4Header, udp4Header);
    }
}