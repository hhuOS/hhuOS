//
// Created by hannes on 17.05.21.
//

#include "ICMP4ReceiveEvent.h"

namespace Kernel {

    String ICMP4ReceiveEvent::getType() const {
        return TYPE;
    }

    ICMP4ReceiveEvent::ICMP4ReceiveEvent(IP4Header *ip4Header, NetworkByteBlock *input) : ip4Header(ip4Header),
                                                                                          input(input) {}

    NetworkByteBlock *ICMP4ReceiveEvent::getInput() const {
        return input;
    }

    IP4Header *ICMP4ReceiveEvent::getIP4Header() const {
        return ip4Header;
    }
}