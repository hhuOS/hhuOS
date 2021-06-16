//
// Created by hannes on 14.06.21.
//

#include <kernel/network/internet/IP4Header.h>
#include "UDP4SocketController.h"

namespace Kernel {
    UDP4SocketController::UDP4SocketController(NetworkEventBus *eventBus) {
        this->eventBus = eventBus;
        receiveLock = new Spinlock();
    }

    uint8_t UDP4SocketController::notifySocket(IP4Header *ip4Header, UDP4Header *udp4Header, NetworkByteBlock *input) {
        if (
                ip4Header== nullptr ||
                udp4Header == nullptr ||
                input == nullptr
                ) {
            return 1;
        }

        this->ip4Header=ip4Header;
        this->udp4Header=udp4Header;
        this->content=input;
        return 0;
    }

    int UDP4SocketController::receive(uint8_t *targetBuffer, size_t length, IP4Header **ip4Header, UDP4Header **udp4Header) {
        if (
                content == nullptr ||
                targetBuffer == nullptr ||
                length==0 ||
                content->bytesRemaining() < length
                ) {
            delete content;
            delete this->ip4Header;
            delete this->udp4Header;
            return -1;
        }

        size_t totalBytesRead = content->bytesRemaining();
        //Cleanup if reading fails
        if(content->read(targetBuffer, length)){
            delete content;
            delete this->ip4Header;
            delete this->udp4Header;
            return -1;
        }

        if(ip4Header== nullptr){
            delete this->ip4Header;
        } else if (this->ip4Header!= nullptr){
            *ip4Header = this->ip4Header;
        }

        if(udp4Header== nullptr){
            delete this->udp4Header;
        } else if (this->udp4Header!= nullptr){
            *udp4Header = this->udp4Header;
        }

        totalBytesRead-=content->bytesRemaining();
        delete content;

        return totalBytesRead;
    }

    uint8_t UDP4SocketController::publishSendEvent(IP4Address *destinationAddress, UDP4Datagram *outDatagram) {
        if (outDatagram == nullptr) {
            return 1;
        }
        eventBus->publish(
                new UDP4SendEvent(destinationAddress, outDatagram)
        );
        return 0;
    }
}