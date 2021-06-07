//
// Created by hannes on 16.05.21.
//

#include "ARPReceiveEvent.h"

Kernel::ARPReceiveEvent::ARPReceiveEvent(ARPMessage *arpMessage, NetworkByteBlock *input) {
    this->arpMessage = arpMessage;
    this->input = input;
}

ARPMessage *Kernel::ARPReceiveEvent::getARPMessage() const {
    return arpMessage;
}

String Kernel::ARPReceiveEvent::getType() const {
    return TYPE;
}

NetworkByteBlock *Kernel::ARPReceiveEvent::getInput() const {
    return input;
}
