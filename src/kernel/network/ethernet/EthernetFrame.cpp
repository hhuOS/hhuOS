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
            ethernetDataPart->length() > ETHERNET_MTU
            ) {
        return 1;
    }
    if (ethernetHeader->copyTo(output)) {
        return 1;
    }
    //Call next level if no errors occurred yet
    if (ethernetDataPart->copyTo(output)) {
        return 1;
    }
    //Add padding if necessary, to reach minimum frame size -> RFC 894 page 1
    uint8_t errors = 0;
    if (output->bytesRemaining() != 0) {
        while (output->bytesRemaining()) {
            errors += output->appendOneByte(0);
        }
    }
    return errors;
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

uint16_t EthernetFrame::length() {
    if (ethernetDataPart == nullptr) {
        return 0;
    }
    uint16_t totalLength = EthernetHeader::length() + ethernetDataPart->length();
    //Return minimum frame size if padding is necessary
    if (totalLength < ETHERNET_MIN_FRAME_SIZE) {
        return ETHERNET_MIN_FRAME_SIZE;
    }
    return totalLength;
}

uint8_t EthernetFrame::setSourceAddress(EthernetAddress *source) {
    if (source == nullptr || ethernetHeader == nullptr) {
        return 1;
    }
    return ethernetHeader->setSourceAddress(source);
}