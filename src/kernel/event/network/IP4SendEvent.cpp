//
// Created by hannes on 14.05.21.
//

#include "IP4SendEvent.h"

namespace Kernel {

    IP4SendEvent::IP4SendEvent(IP4Datagram *datagram) : Event() {
        this->datagram = datagram;
    }

    IP4Datagram *IP4SendEvent::getDatagram() {}

    String IP4SendEvent::getType() const {
        return TYPE;
    }

}