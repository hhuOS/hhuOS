//
// Created by hannes on 14.05.21.
//

#include "UDP4SendEvent.h"

namespace Kernel {

    UDP4SendEvent::UDP4SendEvent(UDP4Datagram *datagram) : Event() {
        this->datagram = datagram;
    }

    UDP4Datagram *UDP4SendEvent::getDatagram() {
        return datagram;
    }

    String UDP4SendEvent::getType() const {
        return TYPE;
    }

}