//
// Created by hannes on 13.06.21.
//

#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/core/System.h>
#include <kernel/service/NetworkService.h>
#include "UDP4Socket.h"
#include "UDP4Datagram.h"

UDP4Socket::UDP4Socket(uint16_t listeningPort) {
    this->networkService = Kernel::System::getService<Kernel::NetworkService>();
    this->listeningPort = listeningPort;
    sendBuffer = new uint8_t[BUFFER_SIZE];
    receiveBuffer = new uint8_t[BUFFER_SIZE];
    networkService->linkEventBus(&this->eventBus);
}

UDP4Socket::UDP4Socket(IP4Address *targetAddress, uint16_t remotePort) {
    this->networkService = Kernel::System::getService<Kernel::NetworkService>();
    this->destinationAddress = targetAddress;
    this->remotePort = remotePort;
    this->listeningPort = 16123;
    sendBuffer = new uint8_t[BUFFER_SIZE];
    receiveBuffer = new uint8_t[BUFFER_SIZE];
    networkService->linkEventBus(&this->eventBus);
}

UDP4Socket::~UDP4Socket() {
    //TODO: Implement check for running send() before deleting!
    delete sendBuffer;
    delete receiveBuffer;
}

uint8_t UDP4Socket::bind() {
    //TODO: Add Spinlock here!
    return networkService->registerListeningPort(listeningPort, receiveBuffer, BUFFER_SIZE);
}

uint8_t UDP4Socket::close() {
    return networkService->unregisterListeningPort(listeningPort);
}

uint8_t UDP4Socket::send(const char *dataBytes, size_t length) {
    if (
            dataBytes == nullptr ||
            destinationAddress == nullptr ||
            length == 0 ||
            eventBus == nullptr ||
            length > BUFFER_SIZE
            ) {
        return 1;
    }
    //Trivial but helpful to centralize access here
    //-> we can filter, convert etc. incoming data here if necessary
    return send((uint8_t *) dataBytes, length);
}

uint8_t UDP4Socket::send(const uint8_t *dataBytes, size_t length) {
    if (
            dataBytes == nullptr ||
            destinationAddress == nullptr ||
            length == 0 ||
            eventBus == nullptr ||
            length > BUFFER_SIZE
            ) {
        return 1;
    }
    //We have no control about incoming data, especially the time when it is deleted
    //-> copy to buffer and sending buffer instead is the only way to make sure it's not deleted before sending!
    for (size_t i = 0; i < length; i++) {
        sendBuffer[i] = dataBytes[i];
    }

    //delete remaining buffer space
    for (size_t i = length; i < BUFFER_SIZE; i++) {
        sendBuffer[i] = 0;
    }

    eventBus->publish(
            new Kernel::UDP4SendEvent(
                    destinationAddress,
                    new UDP4Datagram(listeningPort, remotePort, sendBuffer, length)
            )
    );
    return 0;
}

int UDP4Socket::receive(uint8_t *targetBuffer) {
    //TODO: Implement this one!
    return 0;
}
