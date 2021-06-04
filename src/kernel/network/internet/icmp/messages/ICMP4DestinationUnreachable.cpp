//
// Created by hannes on 17.05.21.
//

#include "ICMP4DestinationUnreachable.h"

uint8_t ICMP4DestinationUnreachable::copyTo(NetworkByteBlock *byteBlock) {
    //TODO: Implement this one!
    return 1;
}

size_t ICMP4DestinationUnreachable::getLengthInBytes() {
    //TODO: Implement this one!
    return 0;
}

ICMP4Message::ICMP4MessageType ICMP4DestinationUnreachable::getICMP4MessageType() {
    return ICMP4MessageType::DESTINATION_UNREACHABLE;
}

uint8_t ICMP4DestinationUnreachable::parse(NetworkByteBlock *input) {
    return 1;
}