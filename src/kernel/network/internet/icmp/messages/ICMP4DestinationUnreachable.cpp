//
// Created by hannes on 17.05.21.
//

#include "ICMP4DestinationUnreachable.h"

ICMP4DestinationUnreachable::ICMP4DestinationUnreachable() {}

ICMP4DestinationUnreachable::ICMP4DestinationUnreachable(IP4DataPart *dataPart) {
    //TODO: Implement parsing from input
}

uint8_t ICMP4DestinationUnreachable::copyDataTo(uint8_t *byteBlock) {
    return nullptr;
}

uint16_t ICMP4DestinationUnreachable::getLengthInBytes() {
    return 0;
}

ICMP4Message::ICMP4MessageType ICMP4DestinationUnreachable::getICMP4MessageType() {
    return ICMP4MessageType::DESTINATION_UNREACHABLE;
}
