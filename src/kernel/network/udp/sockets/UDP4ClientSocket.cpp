//
// Created by hannes on 13.06.21.
//

#include "UDP4ClientSocket.h"

namespace Kernel {
    UDP4ClientSocket::UDP4ClientSocket(IP4Address *targetAddress, uint16_t targetPort) {
        this->destinationAddress = targetAddress;
        this->targetPort = targetPort;
        this->listeningPort = 16123;
        //TODO: Implement find logic for free port number
        networkService = System::getService<NetworkService>();
        controller = networkService->createSocketController();
        bindState=networkService->registerSocketController(listeningPort, controller);
    }

    UDP4ClientSocket::~UDP4ClientSocket() {
        close();
        delete controller;
    }

    uint8_t UDP4ClientSocket::close() {
        return networkService->unregisterSocketController(listeningPort);
    }

    //Client send()
    //-> destination address and remote port should be given via constructor
    uint8_t UDP4ClientSocket::send(void *dataBytes, size_t length) {
        if (
                dataBytes == nullptr ||
                destinationAddress== nullptr ||
                length == 0 ||
                targetPort == 0
                ) {
            return 1;
        }
        auto *byteBlock = new NetworkByteBlock(length);
        if(byteBlock->append(dataBytes,length)){
            delete byteBlock;
            return 1;
        }
        byteBlock->resetIndex();

        controller->publishSendEvent(
                new IP4Address(destinationAddress),
                new UDP4Datagram(this->listeningPort, targetPort, byteBlock)
        );
        return 0;
    }

    //Regular receive() for clients
    uint8_t UDP4ClientSocket::receive(size_t *totalBytesRead, void *targetBuffer, size_t length) {
        if(bindState!=0){
            return 1;
        }
        return controller->receive(totalBytesRead, targetBuffer, length, nullptr, nullptr);
    }

    uint8_t UDP4ClientSocket::receive(void *targetBuffer, size_t length) {
        return receive(nullptr, targetBuffer, length);
    }
}