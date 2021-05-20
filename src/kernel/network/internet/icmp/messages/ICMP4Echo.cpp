//
// Created by hannes on 14.05.21.
//

#include "ICMP4Echo.h"

ICMP4Echo::ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber) {
    myMessage.type = 8; //8 for echo, 0 for echo reply (RFC792)
    myMessage.code = 0;
    myMessage.checksum = 0;
    myMessage.identifier = identifier;
    myMessage.sequenceNumber = sequenceNumber;
}

ICMP4Echo::ICMP4Echo(IP4DataPart *dataPart) {
    auto *input = static_cast<echoMessage *>(dataPart->getMemoryAddress());
    //TODO: Check for valid type
    myMessage.code = input->code;
    myMessage.checksum = input->checksum;
    myMessage.identifier = input->identifier;
    myMessage.sequenceNumber = input->sequenceNumber;
}

void *ICMP4Echo::getMemoryAddress() {
    return &myMessage;
}

uint16_t ICMP4Echo::getLengthInBytes() {
    return sizeof(echoMessage);
}

uint16_t ICMP4Echo::getIdentifier() {
    return myMessage.identifier;
}

uint16_t ICMP4Echo::getSequenceNumber() {
    return myMessage.sequenceNumber;
}
