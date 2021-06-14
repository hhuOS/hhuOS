//
// Created by hannes on 13.06.21.
//

#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/core/System.h>
#include <kernel/service/NetworkService.h>
#include "UDP4Socket.h"

UDP4Socket::UDP4Socket(uint16_t listeningPort) {
    this->networkService = Kernel::System::getService<Kernel::NetworkService>();
    this->listeningPort = listeningPort;
    receiveBuffer = new NetworkByteBlock(BUFFER_SIZE);
    networkService->linkEventBus(&this->eventBus);
}

UDP4Socket::UDP4Socket(IP4Address *targetAddress, uint16_t remotePort) {
    this->networkService = Kernel::System::getService<Kernel::NetworkService>();
    this->destinationAddress = targetAddress;
    this->remotePort = remotePort;
    this->listeningPort = 16123;
    receiveBuffer = new NetworkByteBlock(BUFFER_SIZE);
    networkService->linkEventBus(&this->eventBus);
}

UDP4Socket::~UDP4Socket() {
    //TODO: Implement check for running processing before deleting!
    delete receiveBuffer;
}

uint8_t UDP4Socket::bind() {
    //TODO: Add Spinlock here!
    return networkService->registerListeningPort(listeningPort, receiveBuffer);
}

uint8_t UDP4Socket::close() {
    return networkService->unregisterListeningPort(listeningPort);
}

uint8_t UDP4Socket::send(char *dataBytes, size_t length) {
    if (
            dataBytes == nullptr ||
            destinationAddress == nullptr ||
            length == 0 ||
            eventBus == nullptr
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
            length == 0 ||
            eventBus == nullptr
            ) {
        return 1;
    }

    eventBus->publish(
            new Kernel::UDP4SendEvent(
                    destinationAddress,
                    new UDP4Datagram(listeningPort, remotePort, dataBytes, length)
            )
    );
    return 0;
}

int UDP4Socket::receive(uint8_t *targetBuffer) {
    //TODO: Implement this one!
    return 0;
}
