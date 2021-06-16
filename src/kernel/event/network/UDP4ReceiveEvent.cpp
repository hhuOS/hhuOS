//
// Created by hannes on 17.05.21.
//

#include "UDP4ReceiveEvent.h"

String Kernel::UDP4ReceiveEvent::getType() const {
    return TYPE;
}

Kernel::UDP4ReceiveEvent::UDP4ReceiveEvent(IP4Header *ip4Header, UDP4Header *udp4header, NetworkByteBlock *input) {
    this->udp4Header = udp4header;
    this->ip4Header = ip4Header;
    this->input = input;
}

IP4Header *Kernel::UDP4ReceiveEvent::getIP4Header() const {
    return ip4Header;
}

UDP4Header *Kernel::UDP4ReceiveEvent::getUDP4Header() {
    return udp4Header;
}

NetworkByteBlock *Kernel::UDP4ReceiveEvent::getInput() const {
    return input;
}
