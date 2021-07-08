//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "EthernetFrame.h"

//Private method!
uint8_t EthernetFrame::copyTo(Kernel::NetworkByteBlock *output) {
    if (
            header == nullptr ||
            ethernetDataPart == nullptr ||
            ethernetDataPart->getLengthInBytes() > ETHERNET_MTU
            ) {
        return 1;
    }
    if (header->copyTo(output)) {
        return 1;
    }
    //Call next level if no errors occurred yet
    return ethernetDataPart->copyTo(output);
}

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    this->header = new EthernetHeader(destinationAddress, ethernetDataPart);
    this->ethernetDataPart = ethernetDataPart;
}

EthernetFrame::~EthernetFrame() {
    //delete on nullptr simply does nothing!
    delete header;
    delete ethernetDataPart;
}

size_t EthernetFrame::getLengthInBytes() {
    if (ethernetDataPart == nullptr) {
        return 0;
    }
    return EthernetHeader::getHeaderLength() + ethernetDataPart->getLengthInBytes();
}

uint8_t EthernetFrame::setSourceAddress(EthernetAddress *source) {
    if (source == nullptr || header == nullptr) {
        return 1;
    }
    return header->setSourceAddress(source);
}

String EthernetFrame::headerAsString(const String &spacing) {
    if (header == nullptr) {
        return "NULL";
    }
    return header->asString(spacing);
}