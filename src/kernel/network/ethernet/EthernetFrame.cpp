//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    this->ethernetDataPart = ethernetDataPart;
    destinationAddress->copyTo(header.destinationAddress);

    uint16_t etherTypInt = ethernetDataPart->getEtherTypeAsInt();
    memcpy(header.etherType, &etherTypInt, 2);
}

uint8_t EthernetFrame::copyDataTo(NetworkByteBlock *byteBlock) {
    if (this->ethernetDataPart->getLengthInBytes() > ETHERNETDATAPART_MAX_LENGTH ||
        this->headerLengthInBytes > ETHERNETHEADER_MAX_LENGTH ||
        byteBlock == nullptr) {
        return 1;
    }
    if (byteBlock->writeBytes(&this->header, this->headerLengthInBytes)) {
        return 1;
    }
    return this->ethernetDataPart->copyDataTo(byteBlock);
}

uint16_t EthernetFrame::getTotalLengthInBytes() {
    return this->headerLengthInBytes + this->ethernetDataPart->getLengthInBytes();
}

EthernetFrame::EthernetFrame(void *packet, uint16_t length) {
//TODO:Implement parsing from incoming data
}

EthernetDataPart::EtherType EthernetFrame::getEtherType() const {
    return EthernetDataPart::parseIntAsEtherType((uint16_t) *header.etherType);
}

EthernetDataPart *EthernetFrame::getDataPart() const {
    return ethernetDataPart;
}

void EthernetFrame::setSourceAddress(EthernetAddress *sourceAddress) {
    sourceAddress->copyTo(header.sourceAddress);
}
