//
// Created by hannes on 02.06.21.
//

#include "GenericICMP4Message.h"

GenericICMP4Message::GenericICMP4Message(IP4Address *destinationAddress, IP4Address *sourceAddress,
                                         NetworkByteBlock *input) : destinationAddress(destinationAddress),
                                                                    sourceAddress(sourceAddress),
                                                                    input(input) {}

GenericICMP4Message::GenericICMP4Message(IP4Address *destinationAddress, IP4Address *sourceAddress,
                                         ICMP4Message *message) {
    //TODO: Implement this one!
}

GenericICMP4Message::~GenericICMP4Message() {
    delete this->input;
}

ICMP4Message::ICMP4MessageType GenericICMP4Message::getICMP4MessageType() {
    return this->messageType;
}

uint8_t GenericICMP4Message::parseInput() {
    if (this->input == nullptr) {
        return 1;
    }
    uint8_t messageTypeAsByte = 0;
    this->input->read(&messageTypeAsByte, 1);
    this->messageType = parseByteAsICMP4MessageType(messageTypeAsByte);
    if (this->messageType == ICMP4MessageType::INVALID) {
        return 1;
    }
    return 0;
}

IP4DataPart::IP4ProtocolType GenericICMP4Message::getIP4ProtocolType() {
    return IP4ProtocolType::ICMP4;
}

uint8_t GenericICMP4Message::copyTo(NetworkByteBlock *byteBlock) {
    //This is only for matching incoming data, so no writing to outgoing byte block
    return 1;
}

size_t GenericICMP4Message::getLengthInBytes() {
    return this->input->bytesRemaining();
}

ICMP4Echo *GenericICMP4Message::buildICMP4EchoWithInput() {
    return new ICMP4Echo(this->destinationAddress, this->sourceAddress, this->input);
}

ICMP4EchoReply *GenericICMP4Message::buildICMP4EchoReplyWithInput() {
    return new ICMP4EchoReply(this->destinationAddress, this->sourceAddress, this->input);
}


