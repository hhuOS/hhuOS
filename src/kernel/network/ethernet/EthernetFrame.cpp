//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    destinationAddress->copyTo(header.destinationAddress);
    this->ethernetDataPart = ethernetDataPart;
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

EtherType EthernetFrame::getEtherType() const {
    switch ((uint16_t) *header.etherType) {
        case 0x0800: return EtherType::IP4;
        case 0x0806: return EtherType::ARP;
        case 0x86dd: return EtherType::IP6;
        default: return EtherType::INVALID;
    }
}

EthernetDataPart *EthernetFrame::getDataPart() const {
    return ethernetDataPart;
}

void EthernetFrame::setSourceAddress(EthernetAddress *sourceAddress) {
    sourceAddress->copyTo(header.sourceAddress);
}
