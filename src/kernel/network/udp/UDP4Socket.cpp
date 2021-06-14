//
// Created by hannes on 13.06.21.
//

#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/core/System.h>
#include <kernel/service/NetworkService.h>
#include "UDP4Socket.h"
#include "UDP4Datagram.h"

UDP4Socket::UDP4Socket(uint16_t listeningPort) {
    this->networkService= Kernel::System::getService<Kernel::NetworkService>();
    this->listeningPort = listeningPort;
    receiveBuffer = new uint8_t [1024];
    networkService->linkEventBus(&this->eventBus);
}

UDP4Socket::UDP4Socket(IP4Address *targetAddress, uint16_t remotePort) {
    this->networkService= Kernel::System::getService<Kernel::NetworkService>();
    this->destinationAddress = targetAddress;
    this->remotePort = remotePort;
    this->listeningPort = 16123;
    receiveBuffer = new uint8_t [1024];
    networkService->linkEventBus(&this->eventBus);
}

UDP4Socket::~UDP4Socket() {
    delete receiveBuffer;
}

uint8_t UDP4Socket::bind() {
    //TODO: Add Spinlock here!
    return networkService->registerPort(listeningPort,receiveBuffer,1024);
}

uint8_t UDP4Socket::close() {
    return networkService->unregisterPort(listeningPort);
}

int UDP4Socket::send(uint8_t *bytes, size_t length) {
    if(
            bytes== nullptr ||
            destinationAddress == nullptr ||
            length == 0 ||
            eventBus == nullptr
    ){
        return 1;
    }
    eventBus->publish(
            new Kernel::UDP4SendEvent(
                    destinationAddress,
                    new UDP4Datagram(listeningPort, remotePort, bytes, length)
                    )
    );
    return 0;
}

int UDP4Socket::receive(uint8_t *targetBuffer) {
    //TODO: Implement this one!
    return 0;
}
