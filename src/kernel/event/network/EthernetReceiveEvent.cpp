//
// Created by hannes on 16.05.21.
//

#include "EthernetReceiveEvent.h"

namespace Kernel {
    EthernetReceiveEvent::EthernetReceiveEvent(NetworkByteBlock *input)
            : input(input) {}

    String EthernetReceiveEvent::getType() const {
        return TYPE;
    }

    NetworkByteBlock *EthernetReceiveEvent::getInput() const {
        return input;
    }
}