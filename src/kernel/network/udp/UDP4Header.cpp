//
// Created by hannes on 15.06.21.
//

#include "UDP4Header.h"

UDP4Header::UDP4Header(uint16_t sourcePort, uint16_t destinationPort, Kernel::NetworkByteBlock *dataBytes) {
    this->sourcePort = sourcePort;
    this->destinationPort = destinationPort;
    checksum = 0; //Checksum may be zero, RFC 768 page 3 middle
    length = (uint16_t) UDP4Header::getHeaderLength() + (uint16_t) dataBytes->getLength();
}

uint16_t UDP4Header::getHeaderLength() {
    return 4 * sizeof(uint16_t);
}

uint16_t UDP4Header::getSourcePort() const {
    return sourcePort;
}

uint16_t UDP4Header::getDestinationPort() const {
    return destinationPort;
}

uint16_t UDP4Header::getTotalDatagramLength() const {
    return length;
}

uint8_t UDP4Header::copyTo(Kernel::NetworkByteBlock *output) const {
    uint8_t errors = 0;
    errors += output->appendTwoBytesSwapped(sourcePort);
    errors += output->appendTwoBytesSwapped(destinationPort);
    errors += output->appendTwoBytesSwapped(length);
    errors += output->appendTwoBytesSwapped(checksum);
    return errors;
}

uint8_t UDP4Header::parse(Kernel::NetworkByteBlock *input) {
    if (input->bytesRemaining() <= UDP4Header::getHeaderLength()) {
        return 1;
    }

    uint8_t errors = 0;
    errors += input->readTwoBytesSwappedTo(&sourcePort);
    errors += input->readTwoBytesSwappedTo(&destinationPort);
    errors += input->readTwoBytesSwappedTo(&length);
    errors += input->readTwoBytesSwappedTo(&checksum);

    return errors;
}
