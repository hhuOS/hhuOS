//
// Created by hannes on 17.05.21.
//

#include "ICMPReceiveEvent.h"

Kernel::ICMPReceiveEvent::ICMPReceiveEvent(IP4DataPart *ip4DataPart) {
    this->ip4DataPart=ip4DataPart;
}

IP4DataPart *Kernel::ICMPReceiveEvent::getIp4DataPart() const {
    return ip4DataPart;
}

String Kernel::ICMPReceiveEvent::getType() const {
    return TYPE;
}
