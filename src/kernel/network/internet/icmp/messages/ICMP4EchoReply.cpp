//
// Created by hannes on 17.05.21.
//

#include "ICMP4EchoReply.h"

ICMP4EchoReply::ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber) {
    myMessage.type = 0; //8 for echo, 0 for echo reply (RFC792)
    myMessage.code = 0;
    myMessage.checksum = 0;
    myMessage.identifier = identifier;
    myMessage.sequenceNumber = sequenceNumber;
}

ICMP4EchoReply::ICMP4EchoReply(IP4DataPart *dataPart) {
    auto *input = static_cast<echoReplyMessage *>(dataPart->getMemoryAddress());
    //TODO: Check for valid type
    myMessage.code = input->code;
    myMessage.checksum = input->checksum;
    myMessage.identifier = input->identifier;
    myMessage.sequenceNumber = input->sequenceNumber;
}

void *ICMP4EchoReply::getMemoryAddress() {
    return &myMessage;
}

uint16_t ICMP4EchoReply::getLength() {
    return sizeof(echoReplyMessage);
}

uint16_t ICMP4EchoReply::getIdentifier() {
    return myMessage.identifier;
}

uint16_t ICMP4EchoReply::getSequenceNumber() {
    return myMessage.sequenceNumber;
}
