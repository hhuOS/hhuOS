//
// Created by hannes on 14.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "ICMP4Echo.h"

ICMP4Echo::ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber) {
    echoMessage.checksum = 0; //TODO: Implement checksum calculation
    echoMessage.identifier = identifier;
    echoMessage.sequenceNumber = sequenceNumber;
}

ICMP4Echo::ICMP4Echo(IP4DataPart *dataPart) {
    //TODO: Implement this one!
}

uint8_t ICMP4Echo::copyDataTo(NetworkByteBlock *byteBlock) {
    if (byteBlock == nullptr) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->echoMessage.type,
            sizeof(this->echoMessage.type))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->echoMessage.code,
            sizeof(this->echoMessage.code))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->echoMessage.checksum,
            sizeof(this->echoMessage.checksum))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->echoMessage.identifier,
            sizeof(this->echoMessage.identifier))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->echoMessage.sequenceNumber,
            sizeof(this->echoMessage.sequenceNumber))
            ) {
        return 1;
    }
    return 0;
}

size_t ICMP4Echo::getLengthInBytes() {
    return sizeof(this->echoMessage);
}

uint16_t ICMP4Echo::getIdentifier() const {
    return echoMessage.identifier;
}

uint16_t ICMP4Echo::getSequenceNumber() const {
    return echoMessage.sequenceNumber;
}

ICMP4Message::ICMP4MessageType ICMP4Echo::getICMP4MessageType() {
    return ICMP4MessageType::ECHO;
}

