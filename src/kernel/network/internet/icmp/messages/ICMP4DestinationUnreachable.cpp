//
// Created by hannes on 17.05.21.
//

#include "ICMP4DestinationUnreachable.h"

ICMP4DestinationUnreachable::ICMP4DestinationUnreachable() {}

ICMP4DestinationUnreachable::ICMP4DestinationUnreachable(IP4DataPart *dataPart) {
//TODO: Implement parsing from input
}

void *ICMP4DestinationUnreachable::getMemoryAddress() {
    return nullptr;
}

uint16_t ICMP4DestinationUnreachable::getLength() {
    return 0;
}

IP4ProtocolType ICMP4DestinationUnreachable::getIP4ProtocolType() {
    return IP4ProtocolType::ICMP;
}
