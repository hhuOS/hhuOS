//
// Created by hannes on 13.06.21.
//

#include <kernel/event/network/UDP4SendEvent.h>
#include <kernel/core/System.h>
#include <kernel/service/NetworkService.h>
#include "UDP4Socket.h"

namespace Kernel {
    UDP4Socket::UDP4Socket(uint16_t listeningPort) {
        this->listeningPort = new UDP4Port(listeningPort);
        receiveBuffer = new NetworkByteBlock(BUFFER_SIZE);
        System::getService<NetworkService>()->linkEventBus(&this->eventBus);
    }

    UDP4Socket::UDP4Socket(IP4Address *targetAddress, uint16_t remotePort) {
        this->destinationAddress = targetAddress;
        this->remotePort = new UDP4Port(remotePort);
        this->listeningPort = new UDP4Port(16123);
        receiveBuffer = new NetworkByteBlock(BUFFER_SIZE);
        System::getService<NetworkService>()->linkEventBus(&this->eventBus);
    }

    UDP4Socket::~UDP4Socket() {
        //TODO: Implement check for running processing before deleting!
        delete receiveBuffer;
    }

    uint8_t UDP4Socket::bind() {
        openLock = new Spinlock();
        closeLock = new Spinlock();
        return System::getService<NetworkService>()
                ->registerSocketController(
                        new UDP4SocketController(
                                receiveBuffer,
                                openLock,
                                closeLock
                                )
                        );
    }

    uint8_t UDP4Socket::close() {
        return System::getService<NetworkService>()
                ->unregisterSocketController(listeningPort);
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

    int UDP4Socket::receive(uint8_t *targetBuffer, size_t length) {
        //Block process until opened by incoming data
        openLock->acquire();
        return receiveBuffer->read(targetBuffer, length);
    }
}