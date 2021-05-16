//
// Created by hannes on 16.05.21.
//

#include "EthernetReceiveEvent.h"

Kernel::EthernetReceiveEvent::EthernetReceiveEvent(EthernetFrame *ethernetFrame) : ethernetFrame(ethernetFrame) {}

String Kernel::EthernetReceiveEvent::getType() const {
    return TYPE;
}

EthernetFrame *Kernel::EthernetReceiveEvent::getEthernetFrame() const {
    return ethernetFrame;
}
