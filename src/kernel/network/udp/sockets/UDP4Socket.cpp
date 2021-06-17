//
// Created by hannes on 13.06.21.
//

#include "UDP4Socket.h"

namespace Kernel {
    UDP4Socket::UDP4Socket(uint16_t listeningPort) {
        this->listeningPort=listeningPort;
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController();
    }

    UDP4Socket::UDP4Socket(IP4Address *targetAddress, uint16_t targetPort) {
        this->destinationAddress = targetAddress;
        this->targetPort = targetPort;
        this->listeningPort = 16123;
        //TODO: Implement find logic for free port number
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController();
    }

    UDP4Socket::~UDP4Socket() {
        close();
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
        return send(this->destinationAddress, this->targetPort, dataBytes, length);
    }

    //Server send()
    uint8_t UDP4Socket::send(IP4Address *givenDestination, uint16_t givenRemotePort, void *dataBytes, size_t length) {
        if (
                dataBytes == nullptr ||
                givenDestination == nullptr ||
                length == 0 ||
                givenRemotePort == 0
                ) {
            return 1;
        }
        auto *byteBlock = new NetworkByteBlock(length);
        if(byteBlock->append(dataBytes,length)){
            delete byteBlock;
            return 1;
        }

        controller->publishSendEvent(
                new IP4Address(givenDestination),
                new UDP4Datagram(this->listeningPort, givenRemotePort, byteBlock)
        );
        return 0;
    }

    //Regular receive() for clients
    int UDP4Socket::receive(void *targetBuffer, size_t length) {
        return receive(targetBuffer, length, nullptr, nullptr);
    }

    //Extended receive() for server and clients who need to know IP4 or UDP4 headers
    int UDP4Socket::receive(void *targetBuffer, size_t length, IP4Header **ip4Header, UDP4Header **udp4Header) {
        return controller->receive(targetBuffer, length, ip4Header, udp4Header);
    }
}