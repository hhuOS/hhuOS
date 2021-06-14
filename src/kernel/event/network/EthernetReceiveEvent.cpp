//
// Created by hannes on 16.05.21.
//

#include "EthernetReceiveEvent.h"

namespace Kernel {

    EthernetReceiveEvent::EthernetReceiveEvent(EthernetFrame *ethernetFrame, NetworkByteBlock *input)
            : ethernetFrame(ethernetFrame), input(input) {}

    String EthernetReceiveEvent::getType() const {
        return TYPE;
    }

    EthernetFrame *EthernetReceiveEvent::getEthernetFrame() const {
        return ethernetFrame;
    }

    NetworkByteBlock *EthernetReceiveEvent::getInput() const {
        return input;
    }
}