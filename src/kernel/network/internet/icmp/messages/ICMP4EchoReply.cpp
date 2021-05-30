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
    //TODO: Implement this one!
}

uint8_t ICMP4EchoReply::copyDataTo(NetworkByteBlock *byteBlock) {
    //TODO: Implement this one!
    return 1;
}

uint16_t ICMP4EchoReply::getLengthInBytes() {
    return sizeof(echoReplyMessage);
}

uint16_t ICMP4EchoReply::getIdentifier() {
    return myMessage.identifier;
}

uint16_t ICMP4EchoReply::getSequenceNumber() {
    return myMessage.sequenceNumber;
}

ICMP4Message::ICMP4MessageType ICMP4EchoReply::getICMP4MessageType() {
    return ICMP4MessageType::ECHO_REPLY;
}

void *ICMP4EchoReply::getMemoryAddress() {
    //TODO: Implement this one!
    return nullptr;
}
