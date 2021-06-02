//
// Created by hannes on 02.06.21.
//

#include "GenericICMP4Message.h"

GenericICMP4Message::GenericICMP4Message(IP4Address *destinationAddress, IP4Address *sourceAddress,
                                         NetworkByteBlock *input) : destinationAddress(destinationAddress),
                                                                    sourceAddress(sourceAddress),
                                                                    input(input) {}

GenericICMP4Message::~GenericICMP4Message() {
    delete this->input;
}

ICMP4Message::ICMP4MessageType GenericICMP4Message::getICMP4MessageType() {
    return ICMP4MessageType::INVALID;
}

uint8_t GenericICMP4Message::parseInput() {
    //TODO: Implement this one!
    return 1;
}

