//
// Created by hannes on 16.05.21.
//

#include "IP4ReceiveEvent.h"

namespace Kernel {

    IP4ReceiveEvent::IP4ReceiveEvent(IP4Header *ip4Header, NetworkByteBlock *input) {
        this->ip4Header = ip4Header;
        this->input = input;
    }

    String IP4ReceiveEvent::getType() const {
        return TYPE;
    }

    IP4Header * IP4ReceiveEvent::getHeader() {
        return ip4Header;
    }

    NetworkByteBlock *IP4ReceiveEvent::getInput() const {
        return input;
    }

}