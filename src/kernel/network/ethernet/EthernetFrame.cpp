//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    destinationAddress->copyTo(header.destinationAddress);
    header.etherType = ethernetDataPart->getEtherTypeAsInt();
    this->ethernetDataPart = ethernetDataPart;
}

EthernetFrame::EthernetFrame(NetworkByteBlock *input) {
    this->input = input;
}

uint8_t EthernetFrame::copyDataTo(NetworkByteBlock *byteBlock) {
    if (
        //if initialized with input byteBlock, this method must not continue
            this->ethernetDataPart == nullptr ||
            byteBlock == nullptr ||
            this->ethernetDataPart->getLengthInBytes() > ETHERNETDATAPART_MAX_LENGTH ||
            this->headerLengthInBytes > ETHERNETHEADER_MAX_LENGTH
            ) {
        return 1;
    }
    if (byteBlock->writeBytesStraightFrom(
            &this->header.destinationAddress,
            sizeof this->header.destinationAddress)
            ) {
        return 1;
    }
    if (byteBlock->writeBytesStraightFrom(
            &this->header.sourceAddress,
            sizeof this->header.sourceAddress)
            ) {
        return 1;
    }
    if (byteBlock->writeBytesInNetworkByteOrderFrom(
            &this->header.etherType,
            sizeof this->header.etherType)
            ) {
        return 1;
    }

    return this->ethernetDataPart->copyDataTo(byteBlock);
}

uint16_t EthernetFrame::getTotalLengthInBytes() {
    return this->headerLengthInBytes + this->ethernetDataPart->getLengthInBytes();
}

EthernetDataPart::EtherType EthernetFrame::getEtherType() const {
    return EthernetDataPart::parseIntAsEtherType(header.etherType);
}

void EthernetFrame::setSourceAddress(EthernetAddress *sourceAddress) {
    sourceAddress->copyTo(header.sourceAddress);
}

uint8_t EthernetFrame::parseInput() {
    if (input == nullptr) {
        return 1;
    }
    this->input->resetCurrentIndex();
    if (this->input->readBytesStraightTo(
            &this->header.destinationAddress,
            sizeof(this->header.destinationAddress))
            ) {
        return 1;
    }
    if (this->input->readBytesStraightTo(
            &this->header.sourceAddress,
            sizeof(this->header.sourceAddress))
            ) {
        return 1;
    }
    if (this->input->readBytesInHostByteOrderTo(
            &this->header.etherType,
            sizeof(this->header.etherType))
            ) {
        return 1;
    }
    return 0;
}

EthernetFrame::~EthernetFrame() {
    delete this->input;
}

IP4Datagram *EthernetFrame::buildIP4DatagramWithInput() {
    return new IP4Datagram(this->input);
}

ARPMessage *EthernetFrame::buildARPMessageWithInput() {
    return new ARPMessage(this->input);
}
