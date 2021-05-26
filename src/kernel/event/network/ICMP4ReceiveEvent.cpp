//
// Created by hannes on 17.05.21.
//

#include "ICMP4ReceiveEvent.h"

namespace Kernel {

    ICMP4ReceiveEvent::ICMP4ReceiveEvent(IP4Address *sourceAddress, IP4Address *destinationAddress,
                                         IP4DataPart *ip4DataPart) {
        this->sourceAddress = sourceAddress;
        this->destinationAddress = destinationAddress;
        this->ip4DataPart = ip4DataPart;
    }

    IP4Address *ICMP4ReceiveEvent::getSourceAddress() const {
        return sourceAddress;
    }

    IP4Address *ICMP4ReceiveEvent::getDestinationAddress() const {
        return destinationAddress;
    }

    IP4DataPart *ICMP4ReceiveEvent::getIp4DataPart() const {
        return ip4DataPart;
    }

    String ICMP4ReceiveEvent::getType() const {
        return TYPE;
    }
}