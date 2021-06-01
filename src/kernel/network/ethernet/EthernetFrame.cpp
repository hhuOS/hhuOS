//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    destinationAddress->copyTo(header.destinationAddress);
    header.etherType = ethernetDataPart->getEtherTypeAsInt();
    this->ethernetDataPart = ethernetDataPart;
}

EthernetFrame::EthernetFrame(void *packet, uint16_t length) {
//TODO:Implement parsing from incoming data
}

uint8_t EthernetFrame::copyDataTo(NetworkByteBlock *byteBlock) {
    if (this->ethernetDataPart->getLengthInBytes() > ETHERNETDATAPART_MAX_LENGTH ||
        this->headerLengthInBytes > ETHERNETHEADER_MAX_LENGTH ||
        byteBlock == nullptr) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->header.destinationAddress,
            sizeof this->header.destinationAddress)
            ) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->header.sourceAddress,
            sizeof this->header.sourceAddress)
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
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

EthernetDataPart *EthernetFrame::getDataPart() const {
    return ethernetDataPart;
}

void EthernetFrame::setSourceAddress(EthernetAddress *sourceAddress) {
    sourceAddress->copyTo(header.sourceAddress);
}
