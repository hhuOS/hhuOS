//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include <kernel/network/internet/IP4Datagram.h>
#include <kernel/network/arp/ARPMessage.h>
#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    this->header = new EthernetHeader(destinationAddress, ethernetDataPart);
    this->ethernetDataPart = ethernetDataPart;
}

EthernetFrame::~EthernetFrame() {
    delete header;
    delete ethernetDataPart;
}

size_t EthernetFrame::getLengthInBytes() {
    return EthernetHeader::getHeaderLength() + ethernetDataPart->getLengthInBytes();
}

uint8_t EthernetFrame::setSourceAddress(EthernetAddress *source) {
    if (source == nullptr) {
        return 1;
    }
    return header->setSourceAddress(source);
}

uint8_t EthernetFrame::copyTo(Kernel::NetworkByteBlock *output) {
    if (
            header == nullptr ||
            ethernetDataPart == nullptr ||
            output == nullptr ||
            ethernetDataPart->getLengthInBytes() > ETHERNET_MTU ||
            EthernetHeader::getHeaderLength() > ETHERNETHEADER_MAX_LENGTH
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

String EthernetFrame::headerAsString(const String &spacing) {
    return header->asString(spacing);
}