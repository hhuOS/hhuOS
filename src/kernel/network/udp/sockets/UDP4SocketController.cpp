//
// Created by hannes on 14.06.21.
//

#include "UDP4SocketController.h"

namespace Kernel {
    UDP4SocketController::UDP4SocketController(NetworkEventBus *eventBus, size_t bufferSize) {
        this->eventBus = eventBus;
        receiveBuffer = new NetworkByteBlock(bufferSize);
    }

    uint8_t UDP4SocketController::notifySocket(NetworkByteBlock *input) {
        if (
                receiveBuffer == nullptr ||
                input == nullptr
                ) {
            return 1;
            //TODO: Improve error handling! Simply returning is dangerous with locks...
        }
        receiveBuffer->append(input, input->getLength());
        return 0;
    }

    int UDP4SocketController::receive(uint8_t *targetBuffer, size_t length) {
        if (
                receiveBuffer == nullptr ||
                targetBuffer == nullptr ||
                length > receiveBuffer->getLength()
                ) {
            return -1;
        }
        //TODO: Translate return values
        receiveBuffer->read(targetBuffer, length);
        return 0;
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