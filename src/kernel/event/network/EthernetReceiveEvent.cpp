//
// Created by hannes on 16.05.21.
//

#include "EthernetReceiveEvent.h"

namespace Kernel {

    EthernetReceiveEvent::EthernetReceiveEvent(EthernetFrame *ethernetFrame) : ethernetFrame(ethernetFrame) {}

    String EthernetReceiveEvent::getType() const {
        return TYPE;
    }

    EthernetFrame *EthernetReceiveEvent::getEthernetFrame() const {
        return ethernetFrame;
    }
}