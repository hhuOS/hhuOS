//
// Created by hannes on 14.06.21.
//

#include <kernel/network/internet/IP4Header.h>
#include "UDP4SocketController.h"

namespace Kernel {
    UDP4SocketController::UDP4SocketController(NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        readLock = new Spinlock();
        readLock->acquire();
        writeLock = new Spinlock();
        writeLock->release();
    }

    uint8_t UDP4SocketController::notifySocket(IP4Header *incomingIP4Header, UDP4Header *incomingUDP4Header,
                                               NetworkByteBlock *input) {
        if (
                incomingIP4Header == nullptr ||
                incomingUDP4Header == nullptr ||
                input == nullptr ||
                readLock == nullptr ||
                writeLock == nullptr
                ) {
            return 1;
        }
        writeLock->acquire();
        this->ip4Header = incomingIP4Header;
        this->udp4Header = incomingUDP4Header;
        this->content = input;
        readLock->release();
        return 0;
    }

    uint8_t
    UDP4SocketController::receive(size_t *totalBytesRead, void *targetBuffer, size_t length,
                                  IP4Header **ip4HeaderVariable,
                                  UDP4Header **udp4HeaderVariable) {
        if (readLock == nullptr){
            return 1;
        }
        readLock->acquire();
        if(
            writeLock == nullptr ||
            content == nullptr ||
            targetBuffer == nullptr ||
            length == 0
        ) {
            readLock->release();
            delete content;
            delete this->ip4Header;
            delete this->udp4Header;
            return 1;
        }
        if(totalBytesRead!= nullptr) {
            *totalBytesRead = content->bytesRemaining();
        }
        //Cleanup if reading fails
        if (content->read(targetBuffer, length)) {
            delete content;
            content = nullptr;
            delete this->ip4Header;
            this->ip4Header = nullptr;
            delete this->udp4Header;
            this->udp4Header = nullptr;
            return 1;
        }
        if(totalBytesRead!= nullptr) {
            *totalBytesRead = *totalBytesRead - content->bytesRemaining();
        }

        if (ip4HeaderVariable == nullptr) {
            delete this->ip4Header;
        } else if (this->ip4Header != nullptr) {
            *ip4HeaderVariable = this->ip4Header;
        }

        if (udp4HeaderVariable == nullptr) {
            delete this->udp4Header;
        } else if (this->udp4Header != nullptr) {
            *udp4HeaderVariable = this->udp4Header;
        }

        delete content;
        content = nullptr;
        //Headers are in use somewhere else
        //-> no delete here! Just set them to nullptr to avoid using them again
        this->ip4Header = nullptr;
        this->udp4Header = nullptr;
        readLock->acquire();
        writeLock->release();

        return 0;
    }

    uint8_t UDP4SocketController::publishSendEvent(IP4Address *destinationAddress, UDP4Datagram *outDatagram) {
        if (destinationAddress == nullptr || outDatagram == nullptr) {
            return 1;
        }
        eventBus->publish(
                new UDP4SendEvent(destinationAddress, outDatagram)
        );
        return 0;
    }
}