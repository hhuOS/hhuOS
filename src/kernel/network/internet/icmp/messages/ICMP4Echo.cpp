//
// Created by hannes on 14.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include "ICMP4Echo.h"

ICMP4Echo::ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber) {
    echoMessage.checksum = 0; //TODO: Implement checksum calculation
    echoMessage.identifier = identifier;
    echoMessage.sequenceNumber = sequenceNumber;
}

ICMP4Echo::ICMP4Echo(IP4Address *destinationAddress, IP4Address *sourceAddress, NetworkByteBlock *input) {
    this->destinationAddress = destinationAddress;
    this->sourceAddress = sourceAddress;
    this->input = input;
}

ICMP4Echo::~ICMP4Echo() {
    delete this->input;
}

size_t ICMP4Echo::getLengthInBytes() {
    return sizeof(this->echoMessage);
}

IP4Address *ICMP4Echo::getSourceAddress() const {
    return sourceAddress;
}

ICMP4EchoReply *ICMP4Echo::buildEchoReply() const {
    return new ICMP4EchoReply(this->echoMessage.identifier, this->echoMessage.sequenceNumber + 1);
}

ICMP4Message::ICMP4MessageType ICMP4Echo::getICMP4MessageType() {
    return ICMP4MessageType::ECHO;
}

uint8_t ICMP4Echo::copyDataTo(NetworkByteBlock *byteBlock) {
    if (byteBlock == nullptr) {
        return 1;
    }
    if (byteBlock->writeBytesStraightFrom(
            &this->echoMessage.type,
            sizeof(this->echoMessage.type))
            ) {
        return 1;
    }
    if (byteBlock->writeBytesStraightFrom(
            &this->echoMessage.code,
            sizeof(this->echoMessage.code))
            ) {
        return 1;
    }
    if (byteBlock->writeBytesInNetworkByteOrderFrom(
            &this->echoMessage.checksum,
            sizeof(this->echoMessage.checksum))
            ) {
        return 1;
    }
    if (byteBlock->writeBytesInNetworkByteOrderFrom(
            &this->echoMessage.identifier,
            sizeof(this->echoMessage.identifier))
            ) {
        return 1;
    }
    if (byteBlock->writeBytesInNetworkByteOrderFrom(
            &this->echoMessage.sequenceNumber,
            sizeof(this->echoMessage.sequenceNumber))
            ) {
        return 1;
    }
    return 0;
}

uint8_t ICMP4Echo::parseInput() {
    if (input == nullptr) {
        return 1;
    }
    //NOTE: The first Byte for 'type' is already in our GenericICMP4Message!
    //-> the next Byte in our NetworkByteBlock is 'code', the next value!
    //This is no problem here, because the 'type' value is constant '8' per definition
    if (input->readBytesStraightTo(
            &this->echoMessage.code,
            sizeof(this->echoMessage.code))
            ) {
        return 1;
    }
    if (input->readBytesInHostByteOrderTo(
            &this->echoMessage.checksum,
            sizeof(this->echoMessage.checksum))
            ) {
        return 1;
    }
    if (input->readBytesInHostByteOrderTo(
            &this->echoMessage.identifier,
            sizeof(this->echoMessage.identifier))
            ) {
        return 1;
    }
    if (input->readBytesInHostByteOrderTo(
            &this->echoMessage.sequenceNumber,
            sizeof(this->echoMessage.sequenceNumber))
            ) {
        return 1;
    }
    return 0;
}

