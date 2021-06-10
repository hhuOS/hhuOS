//
// Created by hannes on 17.05.21.
//

#include "UDP4ReceiveEvent.h"

String Kernel::UDP4ReceiveEvent::getType() const {
    return TYPE;
}

Kernel::UDP4ReceiveEvent::UDP4ReceiveEvent(UDP4Datagram *udpDatagram, IP4Datagram *ip4Datagram, NetworkByteBlock *input) {
    this->udpDatagram = udpDatagram;
    this->ip4Datagram=ip4Datagram;
    this->input=input;
}

UDP4Datagram *Kernel::UDP4ReceiveEvent::getUDP4Datagram() {
    return udpDatagram;
}

IP4Datagram *Kernel::UDP4ReceiveEvent::getIP4Datagram() const {
    return ip4Datagram;
}

NetworkByteBlock *Kernel::UDP4ReceiveEvent::getInput() const {
    return input;
}
