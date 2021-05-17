//
// Created by hannes on 17.05.21.
//

#include "UDPReceiveEvent.h"

String Kernel::UDPReceiveEvent::getType() const {
    return TYPE;
}

Kernel::UDPReceiveEvent::UDPReceiveEvent(UDPDatagram *datagram) {
this->datagram=datagram;
}

UDPDatagram *Kernel::UDPReceiveEvent::getDatagram() {
    return datagram;
}
