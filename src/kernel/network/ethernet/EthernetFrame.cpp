//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *dataPart) {
    this->destinationAddress = destinationAddress;
    this->etherType=dataPart->getEtherType();
    this->dataPart=dataPart;
}

void *EthernetFrame::getDataAsByteBlock() {
    return nullptr;
}

uint16_t EthernetFrame::getLength() {
    return 0;
}

EthernetFrame::EthernetFrame(void *packet, uint16_t length) {

}

EthernetAddress *EthernetFrame::getDestinationAddress() const {
    return destinationAddress;
}

EthernetAddress *EthernetFrame::getSourceAddress() const {
    return sourceAddress;
}

EtherType EthernetFrame::getEtherType() const {
    return etherType;
}

EthernetDataPart *EthernetFrame::getDataPart() const {
    return dataPart;
}
