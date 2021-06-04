//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart) {
    destinationAddress->copyTo(header.destinationAddress);
    header.etherType = ethernetDataPart->getEtherTypeAsInt();
    this->ethernetDataPart = ethernetDataPart;
}

EthernetFrame::~EthernetFrame() {
    ethernetDataPart->freeMemory();
}

uint16_t EthernetFrame::getTotalLengthInBytes() {
    return sizeof header + ethernetDataPart->getLengthInBytes();
}

void EthernetFrame::setSourceAddress(EthernetAddress *source) {
    if(source== nullptr){
        return;
    }
    source->copyTo(header.sourceAddress);
}

EthernetDataPart::EtherType EthernetFrame::getEtherType() const {
    return ethernetDataPart->getEtherType();
}

EthernetDataPart *EthernetFrame::getEthernetDataPart() const {
    return ethernetDataPart;
}

uint8_t EthernetFrame::copyTo(NetworkByteBlock *output) {
    if (
            ethernetDataPart == nullptr ||
            output == nullptr ||
            ethernetDataPart->getLengthInBytes() > ETHERNETDATAPART_MAX_LENGTH ||
            sizeof (header) > ETHERNETHEADER_MAX_LENGTH
            ) {
        return 1;
    }

    uint8_t errors=0;
    errors+=output->append(&header.destinationAddress, sizeof header.destinationAddress);
    errors+=output->append(&header.sourceAddress, sizeof header.sourceAddress);
    errors+=output->append(header.etherType);
    errors+= ethernetDataPart->copyTo(output);

    return errors;
}

uint8_t EthernetFrame::parse(NetworkByteBlock *input) {
    if (input == nullptr ||
        input->bytesRemaining() <= sizeof header
        ) {
        return 1;
    }
    uint8_t errors=0;
    errors+=input->read(&header.destinationAddress, MAC_SIZE);
    errors+=input->read(&header.sourceAddress, MAC_SIZE);
    errors+=input->read(&header.etherType);

    switch (EthernetDataPart::parseIntAsEtherType(header.etherType)) {
        case EthernetDataPart::EtherType::IP4: {
            this->ethernetDataPart = new IP4Datagram();
            errors+= ethernetDataPart->parse(input);
            break;
        }
        case EthernetDataPart::EtherType::ARP: {
            this->ethernetDataPart = new ARPMessage();
            errors+= ethernetDataPart->parse(input);
            break;
        }
        default: errors++;
    }

    return errors;
}
