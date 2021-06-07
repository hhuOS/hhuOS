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
    switch (EthernetDataPart::parseIntAsEtherType(header.etherType)) {
        case EthernetDataPart::EtherType::IP4: {
            delete (IP4Datagram *)ethernetDataPart;
            break;
        }
        case EthernetDataPart::EtherType::ARP: {
            delete (ARPMessage *)ethernetDataPart;
            break;
        }
        default: break;
    }
}

uint16_t EthernetFrame::getLengthInBytes() {
    return sizeof header + ethernetDataPart->getLengthInBytes();
}

void EthernetFrame::setSourceAddress(EthernetAddress *source) {
    if (source == nullptr) {
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
            sizeof header > ETHERNETHEADER_MAX_LENGTH
            ) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->append(&header.destinationAddress, sizeof header.destinationAddress);
    errors += output->append(&header.sourceAddress, sizeof header.sourceAddress);
    errors += output->append(header.etherType);

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Call next level if no errors occurred yet
    return ethernetDataPart->copyTo(output);
}

uint8_t EthernetFrame::parse(NetworkByteBlock *input) {
    if (input == nullptr ||
        input->bytesRemaining() <= sizeof header
            ) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&header.destinationAddress, MAC_SIZE);
    errors += input->read(&header.sourceAddress, MAC_SIZE);
    errors += input->read(&header.etherType);

    switch (EthernetDataPart::parseIntAsEtherType(header.etherType)) {
        case EthernetDataPart::EtherType::IP4: {
            ethernetDataPart = new IP4Datagram();
            break;
        }
        case EthernetDataPart::EtherType::ARP: {
            ethernetDataPart = new ARPMessage();
            break;
        }
        default:
            errors++;
    }

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Call next level if no errors occurred yet
    return ethernetDataPart->parse(input);
}
