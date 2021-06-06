//
// Created by hannes on 02.06.21.
//

#include "ARPMessage.h"

ARPMessage::ARPMessage(ARPMessage::OpCode opCode) {
    message.header.opCode = getOpCodeAsInt(opCode);
}

uint8_t ARPMessage::copyTo(NetworkByteBlock *byteBlock) {
    return 0;
}

size_t ARPMessage::getLengthInBytes() {
    return 0;
}

EthernetDataPart::EtherType ARPMessage::getEtherType() {
    return EtherType::ARP;
}

uint8_t ARPMessage::parse(NetworkByteBlock *input) {
    return 1;
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
    return parseOpCodeFromInteger(message.header.opCode);
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

void ARPMessage::freeMemory() {

}
