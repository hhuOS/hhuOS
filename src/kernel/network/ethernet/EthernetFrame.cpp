//
// Created by hannes on 15.05.21.
//

#include <netinet/in.h>
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
            ethernetDataPart == nullptr ||
            byteBlock == nullptr ||
            ethernetDataPart->getLengthInBytes() > ETHERNETDATAPART_MAX_LENGTH ||
            headerLengthInBytes > ETHERNETHEADER_MAX_LENGTH
            ) {
        return 1;
    }
    byteBlock->append(&header.destinationAddress,sizeof header.destinationAddress);
    byteBlock->append(&header.sourceAddress,sizeof header.sourceAddress);

    htons(header.etherType);
    byteBlock->append(&header.etherType,sizeof header.etherType);

    return ethernetDataPart->copyDataTo(byteBlock);
}

uint16_t EthernetFrame::getTotalLengthInBytes() {
    return headerLengthInBytes + ethernetDataPart->getLengthInBytes();
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
    input->resetCurrentIndex();
    if (input->read(
            &header.destinationAddress,
            sizeof(header.destinationAddress))
            ) {
        return 1;
    }
    if (input->read(
            &header.sourceAddress,
            sizeof(header.sourceAddress))
            ) {
        return 1;
    }
    if (input->readBytesInHostByteOrderTo(
            &header.etherType,
            sizeof(header.etherType))
            ) {
        return 1;
    }
    return 0;
}

EthernetFrame::~EthernetFrame() {
    delete input;
}

IP4Datagram *EthernetFrame::buildIP4DatagramWithInput() {
    return new IP4Datagram(input);
}

ARPMessage *EthernetFrame::buildARPMessageWithInput() {
    return new ARPMessage(input);
}
