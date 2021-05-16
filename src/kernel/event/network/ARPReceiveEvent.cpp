//
// Created by hannes on 16.05.21.
//

#include "ARPReceiveEvent.h"

Kernel::ARPReceiveEvent::ARPReceiveEvent(ARPResponse *arpResponse) {
    this->arpResponse=arpResponse;
}

ARPResponse *Kernel::ARPReceiveEvent::getArpResponse() const {
    return arpResponse;
}

String Kernel::ARPReceiveEvent::getType() const {
    return TYPE;
}
