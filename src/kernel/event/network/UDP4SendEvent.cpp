//
// Created by hannes on 10.06.21.
//

#include "UDP4SendEvent.h"

namespace Kernel {
    UDP4SendEvent::UDP4SendEvent(IP4Address *destinationAddress, uint16_t sourcePort, uint16_t destinationPort,
                                 NetworkByteBlock *outData) : Event() {
        this->destinationAddress = destinationAddress;
        this->sourcePort=sourcePort;
        this->destinationPort=destinationPort;
        this->outData = outData;
    }

    String UDP4SendEvent::getType() const {
        return TYPE;
    }

    IP4Address *UDP4SendEvent::getDestinationAddress() {
        return destinationAddress;
    }

    NetworkByteBlock *UDP4SendEvent::getOutData() const {
        return outData;
    }

    uint16_t UDP4SendEvent::getSourcePort() const {
        return sourcePort;
    }

    uint16_t UDP4SendEvent::getDestinationPort() const {
        return destinationPort;
    }
}