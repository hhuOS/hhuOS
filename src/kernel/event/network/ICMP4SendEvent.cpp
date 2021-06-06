//
// Created by hannes on 06.06.21.
//

#include "ICMP4SendEvent.h"

namespace Kernel {
    ICMP4SendEvent::ICMP4SendEvent(IP4Address *destinationAddress, ICMP4Message *icmp4Message) {
        this->destinationAddress = destinationAddress;
        this->icmp4Message = icmp4Message;
    }

    ICMP4Message *ICMP4SendEvent::getIcmp4Message() const {
        return icmp4Message;
    }

    IP4Address *ICMP4SendEvent::getDestinationAddress() const {
        return destinationAddress;
    }

    String ICMP4SendEvent::getType() const {
        return TYPE;
    }
}