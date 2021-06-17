//
// Created by hannes on 17.05.21.
//

#include "UDP4ReceiveEvent.h"

namespace Kernel {
    String UDP4ReceiveEvent::getType() const {
        return TYPE;
    }

    UDP4ReceiveEvent::UDP4ReceiveEvent(IP4Header *ip4Header, UDP4Header *udp4header, NetworkByteBlock *input) {
        this->udp4Header = udp4header;
        this->ip4Header = ip4Header;
        this->input = input;
    }

    IP4Header *UDP4ReceiveEvent::getIP4Header() const {
        return ip4Header;
    }

    UDP4Header *UDP4ReceiveEvent::getUDP4Header() {
        return udp4Header;
    }

    NetworkByteBlock *UDP4ReceiveEvent::getInput() const {
        return input;
    }
}
