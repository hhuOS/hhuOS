//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    this->ethernetDataPart = ethernetDataPart;
    destinationAddress->copyTo(header.destinationAddress);

    uint16_t etherTypInt = ethernetDataPart->getEtherTypeAsInt();
    memcpy(header.etherType,&etherTypInt,2);
}

void *EthernetFrame::getDataAsByteBlock() {
    return nullptr;
}

uint16_t EthernetFrame::getLength() {
    return 0;
}

EthernetFrame::EthernetFrame(void *packet, uint16_t length) {
//TODO:Implement parsing from incoming data
}

EthernetDataPart::EtherType EthernetFrame::getEtherType() const {
    return EthernetDataPart::parseIntAsEtherType((uint16_t)*header.etherType);
}

EthernetDataPart *EthernetFrame::getDataPart() const {
    return ethernetDataPart;
}

void EthernetFrame::setSourceAddress(EthernetAddress *sourceAddress) {
    sourceAddress->copyTo(header.sourceAddress);
}
