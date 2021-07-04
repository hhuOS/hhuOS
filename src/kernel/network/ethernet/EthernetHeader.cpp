//
// Created by hannes on 15.06.21.
//

#include <kernel/network/NetworkDefinitions.h>
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

size_t EthernetHeader::getMaximumFrameLength() {
    return getHeaderLength() + ETHERNET_MTU;
}

uint8_t EthernetHeader::setSourceAddress(EthernetAddress *address) {
    if (address == nullptr) {
        return 1;
    }
    this->sourceAddress = address;
    return 0;
}

uint8_t EthernetHeader::copyTo(Kernel::NetworkByteBlock *output) {
    uint8_t errors = 0;

    uint8_t addressBytes[MAC_SIZE];
    destinationAddress->copyTo(addressBytes);
    errors += output->appendStraightFrom(addressBytes, MAC_SIZE);

    if (errors) {
        return errors;
    }

    sourceAddress->copyTo(addressBytes);
    errors += output->appendStraightFrom(addressBytes, MAC_SIZE);

    if (errors) {
        return errors;
    }

    errors += output->appendTwoBytesSwapped((uint16_t) etherType);
    return errors;
}

uint8_t EthernetHeader::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < getHeaderLength()) {
        return 1;
    }

    uint8_t errors = 0;
    uint8_t addressBytes[MAC_SIZE];

    errors += input->readStraightTo(addressBytes, MAC_SIZE);
    destinationAddress = new EthernetAddress(addressBytes);

    if (errors) {
        return errors;
    }

    errors += input->readStraightTo(addressBytes, MAC_SIZE);
    sourceAddress = new EthernetAddress(addressBytes);

    if (errors) {
        return errors;
    }

    uint16_t typeValue = 0;
    errors += input->readTwoBytesSwappedTo(&typeValue);
    etherType = EthernetDataPart::parseIntAsEtherType(typeValue);

    return errors;
}

bool EthernetHeader::destinationIs(EthernetAddress *otherAddress) {
    if (otherAddress == nullptr || destinationAddress == nullptr) {
        return false;
    }
    return destinationAddress->equals(otherAddress);
}

String EthernetHeader::asString(const String &spacing) {
    return spacing + "Source Address:      " + sourceAddress->asString() + "\n" + spacing + "Destination Address: " +
           destinationAddress->asString() +
           "\n" + spacing + "EtherType:           " + EthernetDataPart::etherTypeAsString(etherType);
}
