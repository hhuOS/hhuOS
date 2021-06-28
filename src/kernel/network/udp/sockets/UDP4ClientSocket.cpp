//
// Created by hannes on 13.06.21.
//

#include <kernel/core/System.h>
#include "UDP4ClientSocket.h"

namespace Kernel {
    UDP4ClientSocket::UDP4ClientSocket(IP4Address *targetAddress, uint16_t targetPort) {
        this->destinationAddress = targetAddress;
        this->targetPort = targetPort;
        this->listeningPort = 16123;
        //TODO: Implement find logic for free port number
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController();
    }

    UDP4ClientSocket::~UDP4ClientSocket() {
        close();
        delete controller;
    }

    uint8_t UDP4ClientSocket::bind() {
        if (controller->startup()) {
            return 1;
        }
        return networkService->registerSocketController(listeningPort, controller);
    }

    uint8_t UDP4ClientSocket::close() {
        //Disable sending and receiving until socket is deleted
        //Errors don't matter here, our controller MUST be unregistered in any case!
        controller->shutdown();
        return networkService->unregisterSocketController(listeningPort);
    }

    uint8_t UDP4ClientSocket::send(void *dataBytes, size_t length) {
        if (
                dataBytes == nullptr ||
                destinationAddress == nullptr ||
                length == 0 ||
                targetPort == 0
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
        auto *destinationAddressCopy = new IP4Address(destinationAddress);
        //Send data via controller to UDP4Module for further processing
        controller->publishSendEvent(destinationAddressCopy, this->listeningPort, targetPort, byteBlock);

        //Datagram will be deleted in EthernetModule after sending
        //-> no 'delete destinationAddressCopy' here!
        return 0;
    }

    //Regular receive() for clients
    uint8_t UDP4ClientSocket::receive(size_t *totalBytesRead, void *targetBuffer, size_t length) {
        return controller->receive(totalBytesRead, targetBuffer, length, nullptr, nullptr);
    }

    uint8_t UDP4ClientSocket::receive(void *targetBuffer, size_t length) {
        return receive(nullptr, targetBuffer, length);
    }
}