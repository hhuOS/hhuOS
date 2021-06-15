//
// Created by hannes on 15.06.21.
//

#include "EthernetHeader.h"

EthernetHeader::EthernetHeader(EthernetAddress *destinationAddress, EthernetDataPart *dataPart) {
    destinationAddress->copyTo(header.destinationAddress);
    header.etherType = dataPart->getEtherTypeAsInt();
    this->destinationAddress = new EthernetAddress(header.destinationAddress);
}

EthernetDataPart::EtherType EthernetHeader::getEtherType() {
    return EthernetDataPart::parseIntAsEtherType(header.etherType);
}

size_t EthernetHeader::getSize() {
    return sizeof header;
}

void EthernetHeader::setSourceAddress(EthernetAddress *address) {
    address->copyTo(header.sourceAddress);
    if (this->sourceAddress != nullptr) {
        return;
    }
    this->sourceAddress = new EthernetAddress(header.sourceAddress);
}

uint8_t EthernetHeader::copyTo(NetworkByteBlock *output) {
    uint8_t errors = 0;
    errors += output->append(header.destinationAddress, MAC_SIZE);
    errors += output->append(header.sourceAddress, MAC_SIZE);
    errors += output->append(header.etherType);
    return errors;
}

uint8_t EthernetHeader::parse(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < getSize()) {
        return 1;
    }

    uint8_t errors = 0;
    errors += input->read(header.destinationAddress, MAC_SIZE);
    errors += input->read(header.sourceAddress, MAC_SIZE);
    errors += input->read(&header.etherType);

    //dataPart is not set in an incoming frame!

    return errors;
    return 0;
}
