//
// Created by hannes on 17.05.21.
//

#include "UDPReceiveEvent.h"

String Kernel::UDPReceiveEvent::getType() const {
    return TYPE;
}

Kernel::UDPReceiveEvent::UDPReceiveEvent(UDPDatagram *udpDatagram, IP4Datagram *ip4Datagram, NetworkByteBlock *input) {
    this->udpDatagram = udpDatagram;
    this->ip4Datagram=ip4Datagram;
    this->input=input;
}

UDPDatagram *Kernel::UDPReceiveEvent::getUDPDatagram() {
    return udpDatagram;
}

IP4Datagram *Kernel::UDPReceiveEvent::getIp4Datagram() const {
    return ip4Datagram;
}

NetworkByteBlock *Kernel::UDPReceiveEvent::getInput() const {
    return input;
}
