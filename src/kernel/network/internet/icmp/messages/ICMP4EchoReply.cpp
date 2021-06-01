//
// Created by hannes on 17.05.21.
//

#include "ICMP4EchoReply.h"

ICMP4EchoReply::ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber) {
    echoReply.type = 0; //8 for echo, 0 for echo reply (RFC792)
    echoReply.code = 0;
    echoReply.checksum = 0;
    echoReply.identifier = identifier;
    echoReply.sequenceNumber = sequenceNumber;
}

ICMP4EchoReply::ICMP4EchoReply(IP4DataPart *dataPart) {
    //TODO: Implement this one!
}

uint8_t ICMP4EchoReply::copyDataTo(NetworkByteBlock *byteBlock) {
    if (byteBlock == nullptr) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->echoReply.type,
            sizeof(this->echoReply.type))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->echoReply.code,
            sizeof(this->echoReply.code))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->echoReply.checksum,
            sizeof(this->echoReply.checksum))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->echoReply.identifier,
            sizeof(this->echoReply.identifier))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->echoReply.sequenceNumber,
            sizeof(this->echoReply.sequenceNumber))
            ) {
        return 1;
    }
    return 0;
}

size_t ICMP4EchoReply::getLengthInBytes() {
    return sizeof(echoReplyMessage);
}

uint16_t ICMP4EchoReply::getIdentifier() {
    return echoReply.identifier;
}

uint16_t ICMP4EchoReply::getSequenceNumber() {
    return echoReply.sequenceNumber;
}

ICMP4Message::ICMP4MessageType ICMP4EchoReply::getICMP4MessageType() {
    return ICMP4MessageType::ECHO_REPLY;
}

void *ICMP4EchoReply::getMemoryAddress() {
    //TODO: Implement this one!
    return nullptr;
}
