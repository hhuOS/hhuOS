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

IP4DataPart::IP4ProtocolType GenericICMP4Message::getIP4ProtocolType() {
    return ICMP4Message::getIP4ProtocolType();
}

uint8_t GenericICMP4Message::copyDataTo(NetworkByteBlock *byteBlock) {
    return ICMP4Message::copyDataTo(byteBlock);
}

size_t GenericICMP4Message::getLengthInBytes() {
    return ICMP4Message::getLengthInBytes();
}

