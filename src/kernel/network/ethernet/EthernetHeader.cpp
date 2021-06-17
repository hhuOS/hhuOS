//
// Created by hannes on 15.06.21.
//

#include "EthernetHeader.h"

EthernetHeader::EthernetHeader(EthernetAddress *destinationAddress, EthernetDataPart *dataPart) {
    this->destinationAddress = destinationAddress;
    this->etherType = dataPart->getEtherType();
}

EthernetHeader::~EthernetHeader() {
    delete destinationAddress;
    delete sourceAddress;
}

EthernetDataPart::EtherType EthernetHeader::getEtherType() const {
    return etherType;
}

size_t EthernetHeader::getHeaderLength() {
    return 2 * MAC_SIZE + sizeof(uint16_t);
}

void EthernetHeader::setSourceAddress(EthernetAddress *address) {
    if (address == nullptr) {
        return;
    }
    this->sourceAddress = address;
}

uint8_t EthernetHeader::copyTo(Kernel::NetworkByteBlock *output) {
    uint8_t errors = 0;

    uint8_t addressBytes[MAC_SIZE];
    destinationAddress->copyTo(addressBytes);
    errors += output->append(addressBytes, MAC_SIZE);

    if (errors) {
        return errors;
    }

    sourceAddress->copyTo(addressBytes);
    errors += output->append(addressBytes, MAC_SIZE);

    if (errors) {
        return errors;
    }

    errors += output->append((uint16_t) etherType);
    return errors;
}

uint8_t EthernetHeader::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < getHeaderLength()) {
        return 1;
    }

    uint8_t errors = 0;
    uint8_t addressBytes[MAC_SIZE];

    errors += input->read(addressBytes, MAC_SIZE);
    destinationAddress = new EthernetAddress(addressBytes);

    if (errors) {
        return errors;
    }

    errors += input->read(addressBytes, MAC_SIZE);
    sourceAddress = new EthernetAddress(addressBytes);

    if (errors) {
        return errors;
    }

    uint16_t typeValue = 0;
    errors += input->read(&typeValue);
    etherType = EthernetDataPart::parseIntAsEtherType(typeValue);

    return errors;
}
