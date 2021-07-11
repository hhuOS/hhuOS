//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "EthernetFrame.h"

//Private method!
uint8_t EthernetFrame::copyTo(Kernel::NetworkByteBlock *output) {
    if (
            ethernetHeader == nullptr ||
            ethernetDataPart == nullptr ||
            ethernetDataPart->getLengthInBytes() > ETHERNET_MTU
            ) {
        return 1;
    }
    if (ethernetHeader->copyTo(output)) {
        return 1;
    }
    //Call next level if no errors occurred yet
    return ethernetDataPart->copyTo(output);
}

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    this->ethernetHeader = new EthernetHeader(destinationAddress, ethernetDataPart);
    this->ethernetDataPart = ethernetDataPart;
}

EthernetFrame::~EthernetFrame() {
    //delete on nullptr simply does nothing!
    delete ethernetHeader;
    delete ethernetDataPart;
}

uint16_t EthernetFrame::getLengthInBytes() {
    if (ethernetDataPart == nullptr) {
        return 0;
    }
    return EthernetHeader::getHeaderLength() + ethernetDataPart->getLengthInBytes();
}

uint8_t EthernetFrame::setSourceAddress(EthernetAddress *source) {
    if (source == nullptr || ethernetHeader == nullptr) {
        return 1;
    }
    return ethernetHeader->setSourceAddress(source);
}