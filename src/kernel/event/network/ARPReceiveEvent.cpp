//
// Created by hannes on 16.05.21.
//

#include "ARPReceiveEvent.h"

namespace Kernel {
    ARPReceiveEvent::ARPReceiveEvent(ARPMessage *arpMessage) {
        this->arpMessage = arpMessage;
    }

    ARPMessage *ARPReceiveEvent::getARPMessage() const {
        return arpMessage;
    }

    String ARPReceiveEvent::getType() const {
        return TYPE;
    }
}