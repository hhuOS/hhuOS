//
// Created by hannes on 18.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "ICMP4TimeExceeded.h"

uint8_t ICMP4TimeExceeded::copyTo(NetworkByteBlock *byteBlock) {
    //TODO: Implement this one!
    return 1;
}

size_t ICMP4TimeExceeded::getLengthInBytes() {
    //TODO: Implement this one!
    return 0;
}

ICMP4Message::ICMP4MessageType ICMP4TimeExceeded::getICMP4MessageType() {
    return ICMP4MessageType::TIME_EXCEEDED;
}

uint8_t ICMP4TimeExceeded::parseHeader(NetworkByteBlock *input) {
    return 1;
}