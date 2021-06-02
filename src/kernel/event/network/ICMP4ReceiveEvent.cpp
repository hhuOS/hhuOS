//
// Created by hannes on 17.05.21.
//

#include "ICMP4ReceiveEvent.h"

namespace Kernel {

    String ICMP4ReceiveEvent::getType() const {
        return TYPE;
    }

    ICMP4ReceiveEvent::ICMP4ReceiveEvent(GenericICMP4Message *genericIcmp4Message) : genericIcmp4Message(
            genericIcmp4Message) {}

    GenericICMP4Message *ICMP4ReceiveEvent::getGenericIcmp4Message() const {
        return genericIcmp4Message;
    }
}