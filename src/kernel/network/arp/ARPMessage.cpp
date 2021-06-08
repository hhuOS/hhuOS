//
// Created by hannes on 02.06.21.
//

#include "ARPMessage.h"

ARPMessage::ARPMessage(ARPMessage::OpCode opCode) {
    header.opCode = getOpCodeAsInt(opCode);
}

uint8_t ARPMessage::copyTo(NetworkByteBlock *output) {
    if (output == nullptr || output->bytesRemaining() > this->getLengthInBytes()) {
        return 1;
    }
    uint8_t errors = 0;
    errors += output->append(header.hardwareType);
    errors += output->append(header.protocolType);
    errors += output->append(header.hardwareAddressLength);
    errors += output->append(header.protocolAddressLength);
    errors += output->append(header.opCode);

    errors += output->append(message.senderHardwareAddress, header.hardwareAddressLength);
    errors += output->append(message.senderProtocolAddress, header.protocolAddressLength);
    errors += output->append(message.targetHardwareAddress, header.hardwareAddressLength);
    errors += output->append(message.targetProtocolAddress, header.protocolAddressLength);

    return errors;
}

size_t ARPMessage::getLengthInBytes() {
    return sizeof header + sizeof message;
}

EthernetDataPart::EtherType ARPMessage::getEtherType() {
    return EtherType::ARP;
}

uint8_t ARPMessage::parseHeader(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < sizeof header) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&header.hardwareType);
    errors += input->read(&header.protocolType);
    errors += input->read(&header.hardwareAddressLength);
    errors += input->read(&header.protocolAddressLength);
    errors += input->read(&header.opCode);

    return errors;
}

uint16_t ARPMessage::getOpCodeAsInt(ARPMessage::OpCode opCode) {
    return (uint16_t) opCode;
}

void ARPMessage::setSenderHardwareAddress(EthernetAddress *senderHardwareAddress) {
    senderHardwareAddress->copyTo(message.senderHardwareAddress);
}

void ARPMessage::setSenderProtocolAddress(IP4Address *senderProtocolAddress) {
    senderProtocolAddress->copyTo(message.senderProtocolAddress);
}

void ARPMessage::setTargetHardwareAddress(EthernetAddress *targetHardwareAddress) {
    targetHardwareAddress->copyTo(message.targetHardwareAddress);
}

void ARPMessage::setTargetProtocolAddress(IP4Address *targetProtocolAddress) {
    targetProtocolAddress->copyTo(message.targetProtocolAddress);
}

ARPMessage::OpCode ARPMessage::getOpCode() const {
    return parseOpCodeFromInteger(header.opCode);
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

uint8_t ARPMessage::parseBody(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() != sizeof message) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(
            message.senderHardwareAddress,
            header.hardwareAddressLength);
    errors += input->read(
            message.senderProtocolAddress,
            header.protocolAddressLength);
    errors += input->read(
            message.targetHardwareAddress,
            header.hardwareAddressLength);
    errors += input->read(
            message.targetProtocolAddress,
            header.protocolAddressLength);

    return errors;
}

uint8_t *ARPMessage::getTargetProtocolAddress() {
    return message.targetProtocolAddress;
}

uint16_t ARPMessage::getProtocolType() const {
    return header.protocolType;
}

uint8_t *ARPMessage::getSenderProtocolAddress() {
    return message.senderProtocolAddress;
}

uint8_t *ARPMessage::getSenderHardwareAddress() {
    return message.senderHardwareAddress;
}

void ARPMessage::copyProtocolAddress(uint8_t *target, const uint8_t *source) const {
    for (size_t i = 0; i < header.protocolAddressLength; i++) {
        target[i] = source[i];
    }
}

void ARPMessage::copyHardwareAddress(uint8_t *target, const uint8_t *source) const {
    for (size_t i = 0; i < header.hardwareAddressLength; i++) {
        target[i] = source[i];
    }
}

ARPMessage *ARPMessage::buildResponse(uint8_t *ourAddressAsBytes) {
    auto *response = new ARPMessage();
    //Same type
    //-> we can access internal attributes directly
    response->header.opCode = getOpCodeAsInt(OpCode::REPLY);
    copyProtocolAddress(
            response->message.targetProtocolAddress,
            this->message.senderProtocolAddress);
    copyProtocolAddress(
            response->message.senderProtocolAddress,
            this->message.targetProtocolAddress);

    copyHardwareAddress(
            response->message.targetHardwareAddress,
            this->message.senderHardwareAddress);
    copyHardwareAddress(
            response->message.senderHardwareAddress,
            ourAddressAsBytes);
    return response;
}
