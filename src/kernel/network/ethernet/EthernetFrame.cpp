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
    //dataPart is null if this frame is an incoming one!
    //-> deleting is only necessary in an outgoing frame
    if (ethernetDataPart == nullptr) {
        return;
    }
    switch (EthernetDataPart::parseIntAsEtherType(header.etherType)) {
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
        return sizeof header;
    }
    return sizeof header + ethernetDataPart->getLengthInBytes();
}

void EthernetFrame::setSourceAddress(EthernetAddress *source) {
    if (source == nullptr) {
        return;
    }
    source->copyTo(header.sourceAddress);
}

EthernetDataPart::EtherType EthernetFrame::getEtherType() const {
    //get EtherType via header, dataPart can be null!
    return EthernetDataPart::parseIntAsEtherType(header.etherType);
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
    errors += output->append(header.destinationAddress, MAC_SIZE);
    errors += output->append(header.sourceAddress, MAC_SIZE);
    errors += output->append(header.etherType);

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Call next level if no errors occurred yet
    return ethernetDataPart->copyTo(output);
}

uint8_t EthernetFrame::parseHeader(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < sizeof header) {
        return 1;
    }

    uint8_t errors = 0;
    errors += input->read(header.destinationAddress, MAC_SIZE);
    errors += input->read(header.sourceAddress, MAC_SIZE);
    errors += input->read(&header.etherType);

    //dataPart is not set in an incoming frame!

    return errors;
}
