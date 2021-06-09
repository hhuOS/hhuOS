//
// Created by hannes on 17.05.21.
//

#include "ICMP4ReceiveEvent.h"

namespace Kernel {

    String ICMP4ReceiveEvent::getType() const {
        return TYPE;
    }

    ICMP4ReceiveEvent::ICMP4ReceiveEvent(IP4Datagram *datagram, NetworkByteBlock *input) : datagram(datagram), input(input) {}

    NetworkByteBlock *ICMP4ReceiveEvent::getInput() const {
        return input;
    }

    IP4Datagram *ICMP4ReceiveEvent::getDatagram() const {
        return datagram;
    }
}