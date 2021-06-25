//
// Created by hannes on 02.06.21.
//

#include "ARPMessage.h"

ARPMessage::ARPMessage(uint16_t hardwareType, uint16_t protocolType, uint8_t hardwareAddressLength,
                       uint8_t protocolAddressLength, OpCode opCode) {
    header.hardwareType = hardwareType;
    header.protocolType = protocolType;

    header.hardwareAddressLength = hardwareAddressLength;
    header.protocolAddressLength = protocolAddressLength;

    header.opCode = getOpCodeAsInt(opCode);

    body.senderHardwareAddress = new uint8_t[header.hardwareAddressLength];
    body.senderProtocolAddress = new uint8_t[header.protocolAddressLength];

    body.targetHardwareAddress = new uint8_t[header.hardwareAddressLength];
    body.targetProtocolAddress = new uint8_t[header.protocolAddressLength];
}

ARPMessage::~ARPMessage() {
    delete body.senderHardwareAddress;
    delete body.senderProtocolAddress;

    delete body.targetHardwareAddress;
    delete body.targetProtocolAddress;
}

size_t ARPMessage::getLengthInBytes() {
    return sizeof header + getBodyLengthInBytes();
}

EthernetDataPart::EtherType ARPMessage::getEtherType() {
    return EtherType::ARP;
}

uint16_t ARPMessage::getOpCodeAsInt(ARPMessage::OpCode opCode) {
    return (uint16_t) opCode;
}

void ARPMessage::setSenderHardwareAddress(uint8_t *bytes) const {
    memcpy(body.senderHardwareAddress, bytes, header.hardwareAddressLength);
}

void ARPMessage::setSenderProtocolAddress(uint8_t *bytes) const {
    memcpy(body.senderProtocolAddress, bytes, header.protocolAddressLength);
}

void ARPMessage::setTargetHardwareAddress(uint8_t *bytes) const {
    memcpy(body.targetHardwareAddress, bytes, header.hardwareAddressLength);
}

void ARPMessage::setTargetProtocolAddress(uint8_t *bytes) const {
    memcpy(body.targetProtocolAddress, bytes, header.protocolAddressLength);
}

ARPMessage::OpCode ARPMessage::getOpCode() const {
    return parseOpCodeFromInteger(header.opCode);
}

[[maybe_unused]] uint8_t *ARPMessage::getTargetHardwareAddress() const {
    return body.targetHardwareAddress;
}

uint8_t *ARPMessage::getTargetProtocolAddress() const {
    return body.targetProtocolAddress;
}

uint16_t ARPMessage::getProtocolType() const {
    return header.protocolType;
}

uint8_t *ARPMessage::getSenderHardwareAddress() const {
    return body.senderHardwareAddress;
}

uint8_t *ARPMessage::getSenderProtocolAddress() const {
    return body.senderProtocolAddress;
}

size_t ARPMessage::getBodyLengthInBytes() const {
    auto twoSize = (size_t) 2;
    return
            twoSize * header.hardwareAddressLength +
            twoSize * header.protocolAddressLength;
}

ARPMessage::OpCode ARPMessage::parseOpCodeFromInteger(uint16_t value) {
    switch (value) {
        case 1:
            return OpCode::REQUEST;
        case 2:
            return OpCode::REPLY;
        default:
            return OpCode::INVALID;
    }
}

uint8_t ARPMessage::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr || output->bytesRemaining() > this->getLengthInBytes()) {
        return 1;
    }
    uint8_t errors = 0;
    errors += output->appendTwoBytesSwapped(header.hardwareType);
    errors += output->appendTwoBytesSwapped(header.protocolType);
    errors += output->appendOneByte(header.hardwareAddressLength);
    errors += output->appendOneByte(header.protocolAddressLength);
    errors += output->appendTwoBytesSwapped(header.opCode);

    errors += output->appendStraightFrom(body.senderHardwareAddress, header.hardwareAddressLength);
    errors += output->appendStraightFrom(body.senderProtocolAddress, header.protocolAddressLength);

    errors += output->appendStraightFrom(body.targetHardwareAddress, header.hardwareAddressLength);
    errors += output->appendStraightFrom(body.targetProtocolAddress, header.protocolAddressLength);

    return errors;
}

uint8_t ARPMessage::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < sizeof header) {
        return 1;
    }
    uint8_t errors = 0;

    errors += input->readTwoBytesSwappedTo(&header.hardwareType);
    errors += input->readTwoBytesSwappedTo(&header.protocolType);
    errors += input->readOneByteTo(&header.hardwareAddressLength);
    errors += input->readOneByteTo(&header.protocolAddressLength);
    errors += input->readTwoBytesSwappedTo(&header.opCode);

    if (errors) {
        return errors;
    }

    body.targetHardwareAddress = new uint8_t[header.hardwareAddressLength];
    body.targetProtocolAddress = new uint8_t[header.protocolAddressLength];

    body.senderHardwareAddress = new uint8_t[header.hardwareAddressLength];
    body.senderProtocolAddress = new uint8_t[header.protocolAddressLength];

    errors += input->readStraightTo(
            body.senderHardwareAddress,
            header.hardwareAddressLength);
    errors += input->readStraightTo(
            body.senderProtocolAddress,
            header.protocolAddressLength);
    errors += input->readStraightTo(
            body.targetHardwareAddress,
            header.hardwareAddressLength);
    errors += input->readStraightTo(
            body.targetProtocolAddress,
            header.protocolAddressLength);

    return errors;
}

ARPMessage *ARPMessage::buildReply(uint8_t *ourAddressAsBytes) const {
    auto *response =
            new ARPMessage(
                    header.hardwareType,
                    header.protocolType,
                    header.hardwareAddressLength,
                    header.protocolAddressLength,
                    OpCode::REPLY
            );
    //TargetHardwareAddress is BROADCAST in Requests
    //-> We don't copy it to response and set it to our own HardwareAddress instead
    memcpy(
            response->body.senderHardwareAddress,
            ourAddressAsBytes,
            this->header.hardwareAddressLength
    );
    memcpy(
            response->body.senderProtocolAddress,
            this->body.targetProtocolAddress,
            this->header.protocolAddressLength
    );
    memcpy(
            response->body.targetHardwareAddress,
            this->body.senderHardwareAddress,
            this->header.hardwareAddressLength
    );
    memcpy(
            response->body.targetProtocolAddress,
            this->body.senderProtocolAddress,
            this->header.protocolAddressLength
    );
    return response;
}

String ARPMessage::asString() {
    return "[ARP Message]";
}
