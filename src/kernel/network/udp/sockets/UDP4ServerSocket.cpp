//
// Created by hannes on 17.06.21.
//

#include "UDP4ServerSocket.h"

namespace Kernel {
    UDP4ServerSocket::UDP4ServerSocket(uint16_t listeningPort) {
        this->listeningPort = listeningPort;
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController(UDP_SOCKET_BUFFER_SIZE);
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
        if (dataBytes == nullptr || givenDestination == nullptr || length == 0 || givenRemotePort == 0) {
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
        //Send data via controller to UDP4Module for further processing (if successful)
        if (controller->publishSendEvent(givenDestinationCopy, this->listeningPort, givenRemotePort, byteBlock)) {
            delete givenDestination;
            delete byteBlock;
            return 1;
        }

        //Datagram will be deleted in EthernetModule after sending
        //-> no 'delete destinationAddressCopy' here!
        return 0;
    }

    uint8_t UDP4ServerSocket::receive(size_t *totalBytesRead, void *targetBuffer, size_t length) {
        return receive(totalBytesRead, targetBuffer, length, nullptr, nullptr);
    }

    uint8_t UDP4ServerSocket::receive(void *targetBuffer, size_t length) {
        return receive(nullptr, targetBuffer, length, nullptr, nullptr);
    }

    //Extended receive() for servers and clients who need to know IP4 or UDP4 headers
    uint8_t
    UDP4ServerSocket::receive(size_t *totalBytesRead, void *targetBuffer, size_t length, IP4Header **ip4HeaderVariable,
                              UDP4Header **udp4HeaderVariable) {
        if (controller->receive(totalBytesRead, targetBuffer, length, ip4HeaderVariable, udp4HeaderVariable)) {
            if (totalBytesRead != nullptr) {
                *totalBytesRead = 0;
            }
            if (udp4HeaderVariable != nullptr) {
                *udp4HeaderVariable = nullptr;
            }
            if (ip4HeaderVariable != nullptr) {
                *ip4HeaderVariable = nullptr;
            }
            return 1;
        }
        return 0;
    }
}