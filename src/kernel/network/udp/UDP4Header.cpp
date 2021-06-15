//
// Created by hannes on 15.06.21.
//

#include "UDP4Header.h"

UDP4Header::UDP4Header(UDP4Port *sourcePort, UDP4Port *destinationPort, uint16_t length, uint16_t checksum) {
    this->sourcePort = sourcePort;
    this->destinationPort = destinationPort;

    header.length = length;
    header.checksum = checksum;

    //create ports once here, delete with this object
    //-> getter don't create new objects each time
    //-> reduces number of new() calls
    sourcePort->copyTo(header.sourcePort);
    destinationPort->copyTo(header.destinationPort);
}

UDP4Header::~UDP4Header() {
    delete sourcePort;
    delete destinationPort;
}

size_t UDP4Header::getSize() {
    return sizeof header;
}

UDP4Port *UDP4Header::getSourcePort() const {
    return sourcePort;
}

UDP4Port *UDP4Header::getDestinationPort() const {
    return destinationPort;
}

uint8_t UDP4Header::parse(NetworkByteBlock *input) {
    if(sourcePort != nullptr || destinationPort != nullptr){
        //Already initialized!
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

uint8_t UDP4Header::copyTo(NetworkByteBlock *output) {
    uint8_t errors = 0;
    errors += output->append(header.sourcePort);
    errors += output->append(header.destinationPort);
    errors += output->append(header.length);
    errors += output->append(header.checksum);
    return errors;
}
