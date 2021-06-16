//
// Created by hannes on 15.06.21.
//

#include "UDP4Header.h"

UDP4Header::UDP4Header(UDP4Port *sourcePort, UDP4Port *destinationPort, NetworkByteBlock *dataBytes) {
    header.length = sizeof header + dataBytes->getLength();
    header.checksum = 0; //Checksum may be zero, RFC 768 page 3 middle

    //create ports once here, delete with this object
    //-> getter don't create new objects each time
    //-> reduces number of new() calls
    sourcePort->copyTo(&header.sourcePort);
    destinationPort->copyTo(&header.destinationPort);

    this->sourcePort = new UDP4Port(header.sourcePort);
    this->destinationPort = new UDP4Port(header.destinationPort);
}

UDP4Header::~UDP4Header() {
    delete sourcePort;
    delete destinationPort;
}

size_t UDP4Header::getHeaderSize() {
    return sizeof header;
}

UDP4Port *UDP4Header::getSourcePort() const {
    return sourcePort;
}

UDP4Port *UDP4Header::getDestinationPort() const {
    return destinationPort;
}

size_t UDP4Header::getDatagramLength() const {
    return (size_t) header.length;
}

uint8_t UDP4Header::copyTo(NetworkByteBlock *output) const {
    uint8_t errors = 0;
    errors += output->append(header.sourcePort);
    errors += output->append(header.destinationPort);
    errors += output->append(header.length);
    errors += output->append(header.checksum);
    return errors;
}

uint8_t UDP4Header::parse(NetworkByteBlock *input) {
    if (
            sourcePort != nullptr ||
            destinationPort != nullptr ||
            input->bytesRemaining() <= sizeof header
            ) {
        return 1;
    }

    uint8_t errors = 0;
    errors += input->read(&header.sourcePort);
    errors += input->read(&header.destinationPort);
    errors += input->read(&header.length);
    errors += input->read(&header.checksum);

    sourcePort = new UDP4Port(header.sourcePort);
    destinationPort = new UDP4Port(header.destinationPort);

    return errors;
}
