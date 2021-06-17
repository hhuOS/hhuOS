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

uint8_t *ARPMessage::getTargetProtocolAddress() const {
    return body.targetProtocolAddress;
}

uint16_t ARPMessage::getProtocolType() const {
    return header.protocolType;
}

uint8_t *ARPMessage::getSenderProtocolAddress() const {
    return body.senderProtocolAddress;
}

uint8_t *ARPMessage::getSenderHardwareAddress() const {
    return body.senderHardwareAddress;
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
    errors += output->append(header.hardwareType);
    errors += output->append(header.protocolType);
    errors += output->append(header.hardwareAddressLength);
    errors += output->append(header.protocolAddressLength);
    errors += output->append(header.opCode);

    errors += output->append(body.senderHardwareAddress, header.hardwareAddressLength);
    errors += output->append(body.senderProtocolAddress, header.protocolAddressLength);

    errors += output->append(body.targetHardwareAddress, header.hardwareAddressLength);
    errors += output->append(body.targetProtocolAddress, header.protocolAddressLength);

    return errors;
}

uint8_t ARPMessage::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() != getLengthInBytes()) {
        return 1;
    }
    uint8_t errors = 0;

    errors += input->read(&header.hardwareType);
    errors += input->read(&header.protocolType);
    errors += input->read(&header.hardwareAddressLength);
    errors += input->read(&header.protocolAddressLength);
    errors += input->read(&header.opCode);

    body.targetHardwareAddress = new uint8_t[header.hardwareAddressLength];
    body.targetProtocolAddress = new uint8_t[header.protocolAddressLength];

    body.senderHardwareAddress = new uint8_t[header.hardwareAddressLength];
    body.senderProtocolAddress = new uint8_t[header.protocolAddressLength];

    errors += input->read(
            body.senderProtocolAddress,
            header.protocolAddressLength);
    errors += input->read(
            body.targetHardwareAddress,
            header.hardwareAddressLength);
    errors += input->read(
            body.targetProtocolAddress,
            header.protocolAddressLength);

    errors += input->read(
            body.senderHardwareAddress,
            header.hardwareAddressLength);
    errors += input->read(
            body.senderProtocolAddress,
            header.protocolAddressLength);
    errors += input->read(
            body.targetHardwareAddress,
            header.hardwareAddressLength);
    errors += input->read(
            body.targetProtocolAddress,
            header.protocolAddressLength);

    return errors;
}

ARPMessage *ARPMessage::buildResponse(uint8_t *ourAddressAsBytes) const {
    auto *response = new ARPMessage();
    //Same type
    //-> we can access internal attributes directly
    response->header.opCode = getOpCodeAsInt(OpCode::REPLY);
    //TargetHardwareAddress is BROADCAST in Requests
    //-> We don't copy it to response and set it to our HardwareAddress instead
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
