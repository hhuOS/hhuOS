//
// Created by hannes on 17.05.21.
//

#include "ICMP4ReceiveEvent.h"

namespace Kernel {

    String ICMP4ReceiveEvent::getType() const {
        return TYPE;
    }

    ICMP4ReceiveEvent::ICMP4ReceiveEvent(ICMP4Message *icmp4Message, NetworkByteBlock *input) : icmp4Message(
            icmp4Message) {}

    ICMP4Message * ICMP4ReceiveEvent::getIcmp4Message() const {
        return icmp4Message;
    }

    NetworkByteBlock *ICMP4ReceiveEvent::getInput() const {
        return input;
    }
}