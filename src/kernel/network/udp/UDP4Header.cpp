//
// Created by hannes on 15.06.21.
//

#include "UDP4Header.h"

UDP4Header::UDP4Header(UDP4Port *sourcePort, UDP4Port *destinationPort, NetworkByteBlock *dataBytes) {
    this->sourcePort = sourcePort;
    this->destinationPort = destinationPort;

    header.length = dataBytes->getLength() + sizeof header;
    header.checksum = calculateChecksum(dataBytes);

    //create ports once here, delete with this object
    //-> getter don't create new objects each time
    //-> reduces number of new() calls
    sourcePort->copyTo(&header.sourcePort);
    destinationPort->copyTo(&header.destinationPort);
}

UDP4Header::~UDP4Header() {
    delete sourcePort;
    delete destinationPort;
}

bool UDP4Header::checksumCorrect(NetworkByteBlock *input){
    return calculateChecksum(input) == header.checksum;
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

size_t UDP4Header::getDatagramLength() {
    return (size_t)header.length;
}

uint8_t UDP4Header::copyTo(NetworkByteBlock *output) {
    uint8_t errors = 0;
    errors += output->append(header.sourcePort);
    errors += output->append(header.destinationPort);
    errors += output->append(header.length);
    errors += output->append(header.checksum);
    return errors;
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

uint16_t UDP4Header::calculateChecksum(NetworkByteBlock *input) {
    //TODO: Implement this one!
    return 0;
}
