//
// Created by hannes on 10.06.21.
//

#include "UDP4SendEvent.h"

namespace Kernel {

    UDP4SendEvent::UDP4SendEvent(IP4Address *destinationAddress, UDP4Datagram *datagram) : Event() {
        this->destinationAddress = destinationAddress;
        this->datagram = datagram;
    }

    UDP4Datagram *UDP4SendEvent::getDatagram() {
        return datagram;
    }

    String UDP4SendEvent::getType() const {
        return TYPE;
    }

    IP4Address *UDP4SendEvent::getDestinationAddress() {
        return destinationAddress;
    }

}