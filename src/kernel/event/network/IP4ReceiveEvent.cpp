//
// Created by hannes on 16.05.21.
//

#include "IP4ReceiveEvent.h"

namespace Kernel {

    IP4ReceiveEvent::IP4ReceiveEvent(IP4Datagram *datagram) {
        this->datagram = datagram;
    }

    String IP4ReceiveEvent::getType() const {
        return TYPE;
    }

    IP4Datagram *IP4ReceiveEvent::getDatagram() {
        return datagram;
    }

}