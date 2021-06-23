//
// Created by hannes on 16.05.21.
//

#include "IP4ReceiveEvent.h"

namespace Kernel {
    IP4ReceiveEvent::IP4ReceiveEvent(NetworkByteBlock *input) {
        this->input = input;
    }

    String IP4ReceiveEvent::getType() const {
        return TYPE;
    }

    NetworkByteBlock *IP4ReceiveEvent::getInput() const {
        return input;
    }
}