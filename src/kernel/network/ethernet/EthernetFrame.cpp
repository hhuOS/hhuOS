//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    this->destinationAddress = destinationAddress;
    this->ethernetDataPart = ethernetDataPart;
}

EthernetFrame::EthernetFrame(NetworkByteBlock *input) {
    this->input = input;
}

EthernetFrame::~EthernetFrame() {
    delete destinationAddress;
    delete sourceAddress;
    delete input;
    ethernetDataPart->freeMemory();
}

uint16_t EthernetFrame::getTotalLengthInBytes() {
    return sizeof (ethernetHeader) + ethernetDataPart->getLengthInBytes();
}

void EthernetFrame::setSourceAddress(EthernetAddress *source) {
    this->sourceAddress = source;
}

EthernetDataPart::EtherType EthernetFrame::getEtherType() const {
    return ethernetDataPart->getEtherType();
}

uint8_t EthernetFrame::copyDataTo(NetworkByteBlock *byteBlock) {
    if (
            ethernetDataPart == nullptr ||
            byteBlock == nullptr ||
            ethernetDataPart->getLengthInBytes() > ETHERNETDATAPART_MAX_LENGTH ||
            sizeof (ethernetHeader) > ETHERNETHEADER_MAX_LENGTH
            ) {
        return 1;
    }
    destinationAddress->copyTo(header.destinationAddress);
    sourceAddress->copyTo(header.sourceAddress);
    header.etherType = ethernetDataPart->getEtherTypeAsInt();

    uint8_t errors=0;
    errors+=byteBlock->append(&header.destinationAddress, sizeof header.destinationAddress);
    errors+=byteBlock->append(&header.sourceAddress, sizeof header.sourceAddress);
    errors+=byteBlock->append(header.etherType);
    errors+=ethernetDataPart->copyDataTo(byteBlock);

    return errors;
}

uint8_t EthernetFrame::parseInput() {
    if (input == nullptr) {
        return 1;
    }
    uint8_t errors=0;
    errors+=input->read(&header.destinationAddress, MAC_SIZE);
    errors+=input->read(&header.sourceAddress, MAC_SIZE);
    errors+=input->read(&header.etherType);

    switch (EthernetDataPart::parseIntAsEtherType(header.etherType)) {
        case EthernetDataPart::EtherType::IP4: {
            this->ethernetDataPart = new IP4Datagram(input);
            errors+=ethernetDataPart->parseInput();
            break;
        }
        case EthernetDataPart::EtherType::ARP: {
            this->ethernetDataPart = new ARPMessage(input);
            errors+=ethernetDataPart->parseInput();
            break;
        }
        default: errors++;
    }

    return errors;
}

EthernetDataPart *EthernetFrame::getEthernetDataPart() const {
    return ethernetDataPart;
}
