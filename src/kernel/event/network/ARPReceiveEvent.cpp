//
// Created by hannes on 16.05.21.
//

#include "ARPReceiveEvent.h"

namespace Kernel {
    ARPReceiveEvent::ARPReceiveEvent(NetworkByteBlock *input) {
        this->input = input;
    }

    String ARPReceiveEvent::getType() const {
        return TYPE;
    }

    NetworkByteBlock *ARPReceiveEvent::getInput() const {
        return input;
    }
}