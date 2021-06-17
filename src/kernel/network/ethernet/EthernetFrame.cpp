//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    this->ethernetDataPart = ethernetDataPart;
    this->header = new EthernetHeader(destinationAddress, ethernetDataPart);
}

EthernetFrame::~EthernetFrame() {
    //dataPart is null if this frame is an incoming one!
    //-> deleting is only necessary in an outgoing frame
    if (ethernetDataPart == nullptr) {
        return;
    }
    switch (header->getEtherType()) {
        case EthernetDataPart::EtherType::IP4: {
            delete (IP4Datagram *) ethernetDataPart;
            break;
        }
        case EthernetDataPart::EtherType::ARP: {
            delete (ARPMessage *) ethernetDataPart;
            break;
        }
        default:
            break;
    }
}

uint16_t EthernetFrame::getLengthInBytes() {
    if (ethernetDataPart == nullptr) {
        return static_cast<uint16_t>(header->getSize());
    }
    return static_cast<uint16_t>(header->getSize() + ethernetDataPart->getLengthInBytes());
}

void EthernetFrame::setSourceAddress(EthernetAddress *source) {
    if (source == nullptr) {
        return;
    }
    header->setSourceAddress(source);
}

uint8_t EthernetFrame::copyTo(Kernel::NetworkByteBlock *output) {
    if (
            ethernetDataPart == nullptr ||
            output == nullptr ||
            ethernetDataPart->getLengthInBytes() > ETHERNETDATAPART_MAX_LENGTH ||
            header->getSize() > ETHERNETHEADER_MAX_LENGTH
            ) {
        return 1;
    }

    uint8_t errors = header->copyTo(output);

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Call next level if no errors occurred yet
    return ethernetDataPart->copyTo(output);
}