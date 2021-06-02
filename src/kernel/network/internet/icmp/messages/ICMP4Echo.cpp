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

ICMP4Echo::ICMP4Echo(NetworkByteBlock *input) {
    this->input = input;
}

ICMP4Echo::~ICMP4Echo() {
    delete this->input;
}

uint8_t ICMP4Echo::copyDataTo(NetworkByteBlock *input) {
    if (input == nullptr) {
        return 1;
    }
    if (input->appendBytesStraight(
            &this->echoMessage.type,
            sizeof(this->echoMessage.type))
            ) {
        return 1;
    }
    if (input->appendBytesStraight(
            &this->echoMessage.code,
            sizeof(this->echoMessage.code))
            ) {
        return 1;
    }
    if (input->appendBytesInNetworkByteOrder(
            &this->echoMessage.checksum,
            sizeof(this->echoMessage.checksum))
            ) {
        return 1;
    }
    if (input->appendBytesInNetworkByteOrder(
            &this->echoMessage.identifier,
            sizeof(this->echoMessage.identifier))
            ) {
        return 1;
    }
    if (input->appendBytesInNetworkByteOrder(
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

uint8_t ICMP4Echo::parseInput() {
    if (input == nullptr) {
        return 1;
    }
    //NOTE: The first Byte for 'type' is already in our GenericICMP4Message!
    //-> the next Byte in our NetworkByteBlock is 'code', the next value!
    //This is no problem here, because the 'type' value is constant '8' per definition
    if (input->writeBytesStraightTo(
            &this->echoMessage.code,
            sizeof(this->echoMessage.code))
            ) {
        return 1;
    }
    if (input->writeBytesInHostByteOrderTo(
            &this->echoMessage.checksum,
            sizeof(this->echoMessage.checksum))
            ) {
        return 1;
    }
    if (input->writeBytesInHostByteOrderTo(
            &this->echoMessage.identifier,
            sizeof(this->echoMessage.identifier))
            ) {
        return 1;
    }
    if (input->writeBytesInHostByteOrderTo(
            &this->echoMessage.sequenceNumber,
            sizeof(this->echoMessage.sequenceNumber))
            ) {
        return 1;
    }
    return 0;
}

