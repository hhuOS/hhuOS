//
// Created by hannes on 16.05.21.
//

#include "EthernetReceiveEvent.h"

namespace Kernel {

    EthernetReceiveEvent::EthernetReceiveEvent(EthernetHeader *ethernetHeader, NetworkByteBlock *input)
            : ethernetHeader(ethernetHeader), input(input) {}

    String EthernetReceiveEvent::getType() const {
        return TYPE;
    }

    EthernetHeader * EthernetReceiveEvent::getEthernetHeader() const {
        return ethernetHeader;
    }

    NetworkByteBlock *EthernetReceiveEvent::getInput() const {
        return input;
    }
}