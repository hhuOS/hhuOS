//
// Created by hannes on 16.05.21.
//

#include "ARPReceiveEvent.h"

Kernel::ARPReceiveEvent::ARPReceiveEvent(ARPMessage *arpMessage) {
    this->arpMessage = arpMessage;
}

ARPMessage *Kernel::ARPReceiveEvent::getARPMessage() const {
    return arpMessage;
}

String Kernel::ARPReceiveEvent::getType() const {
    return TYPE;
}
