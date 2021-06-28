//
// Created by hannes on 17.06.21.
//

#include <kernel/core/System.h>
#include "UDP4ServerSocket.h"

namespace Kernel {
    UDP4ServerSocket::UDP4ServerSocket(uint16_t listeningPort) {
        this->listeningPort = listeningPort;
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController();
    }

    UDP4ServerSocket::~UDP4ServerSocket() {
        close();
        delete controller;
    }

    uint8_t UDP4ServerSocket::bind() {
        if (controller->startup()) {
            return 1;
        }
        return networkService->registerSocketController(listeningPort, controller);
    }

    uint8_t UDP4ServerSocket::close() {
        //Disable sending and receiving until socket is deleted
        //Errors don't matter here, our controller MUST be unregistered in any case!
        controller->shutdown();
        return networkService->unregisterSocketController(listeningPort);
    }

    uint8_t
    UDP4ServerSocket::send(IP4Address *givenDestination, uint16_t givenRemotePort, void *dataBytes, size_t length) {
        if (
                dataBytes == nullptr ||
                givenDestination == nullptr ||
                length == 0 ||
                givenRemotePort == 0
                ) {
            return 1;
        }
        auto *byteBlock = new NetworkByteBlock(length);
        if (byteBlock->appendStraightFrom(dataBytes, length)) {
            delete byteBlock;
            return 1;
        }
        byteBlock->resetIndex();

        //The datagram's attributes will be deleted after sending
        //-> copy it here!
        auto *givenDestinationCopy = new IP4Address(givenDestination);
        controller->publishSendEvent(givenDestinationCopy, this->listeningPort, givenRemotePort, byteBlock);
        return 0;
    }

    //Extended receive() for servers and clients who need to know IP4 or UDP4 headers
    uint8_t UDP4ServerSocket::receive(size_t *totalBytesRead, void *targetBuffer, size_t length, IP4Header **ip4Header,
                                      UDP4Header **udp4Header) {
        return controller->receive(totalBytesRead, targetBuffer, length, ip4Header, udp4Header);
    }
}