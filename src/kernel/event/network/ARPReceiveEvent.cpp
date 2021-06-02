//
// Created by hannes on 16.05.21.
//

#include "ARPReceiveEvent.h"

Kernel::ARPReceiveEvent::ARPReceiveEvent(ARPMessage *arpMessage) {
    this->arpResponse = arpMessage;
}

ARPMessage *Kernel::ARPReceiveEvent::getARPMessage() const {
    return arpResponse;
}

String Kernel::ARPReceiveEvent::getType() const {
    return TYPE;
}
