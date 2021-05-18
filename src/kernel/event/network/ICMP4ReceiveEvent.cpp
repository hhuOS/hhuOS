//
// Created by hannes on 17.05.21.
//

#include "ICMP4ReceiveEvent.h"

namespace Kernel {

    ICMP4ReceiveEvent::ICMP4ReceiveEvent(IP4DataPart *ip4DataPart) {
        this->ip4DataPart = ip4DataPart;
    }

    IP4DataPart *ICMP4ReceiveEvent::getIp4DataPart() const {
        return ip4DataPart;
    }

    String ICMP4ReceiveEvent::getType() const {
        return TYPE;
    }

}