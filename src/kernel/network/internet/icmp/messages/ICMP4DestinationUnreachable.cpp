//
// Created by hannes on 17.05.21.
//

#include "ICMP4DestinationUnreachable.h"

ICMP4DestinationUnreachable::ICMP4DestinationUnreachable() {}

ICMP4DestinationUnreachable::ICMP4DestinationUnreachable(IP4DataPart *dataPart) {
    //TODO: Implement parsing from input
}

uint8_t ICMP4DestinationUnreachable::copyDataTo(NetworkByteBlock *byteBlock) {
    //TODO: Implement this one!
    return 1;
}

uint16_t ICMP4DestinationUnreachable::getLengthInBytes() {
    //TODO: Implement this one!
    return 0;
}

ICMP4Message::ICMP4MessageType ICMP4DestinationUnreachable::getICMP4MessageType() {
    return ICMP4MessageType::DESTINATION_UNREACHABLE;
}

void *ICMP4DestinationUnreachable::getMemoryAddress() {
    //TODO: Implement this one!
    return nullptr;
}
