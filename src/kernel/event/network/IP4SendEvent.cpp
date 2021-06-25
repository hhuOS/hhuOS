//
// Created by hannes on 14.05.21.
//

#include "IP4SendEvent.h"

namespace Kernel {
    IP4SendEvent::IP4SendEvent(IP4Address *destinationAddress, IP4DataPart *dataPart) : Event() {
        this->destinationAddress=destinationAddress;
        this->dataPart=dataPart;
    }

    String IP4SendEvent::getType() const {
        return TYPE;
    }

    IP4Address *IP4SendEvent::getDestinationAddress() const {
        return destinationAddress;
    }

    IP4DataPart *IP4SendEvent::getDataPart() const {
        return dataPart;
    }
}