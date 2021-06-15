//
// Created by hannes on 17.05.21.
//

#include <lib/libc/printf.h>
#include "UDP4Datagram.h"

UDP4Datagram::UDP4Datagram(UDP4Port *sourcePort, UDP4Port *destinationPort, uint8_t *outgoingBytes, size_t length) {
    this->dataBytes=new NetworkByteBlock(length);
    this->dataBytes->append(outgoingBytes, length);
    this->dataBytes->resetIndex();

    this->header= new UDP4Header(sourcePort, destinationPort, getLengthInBytes(), 0);
}

UDP4Datagram::~UDP4Datagram() {
    //delete on nullptr simply does nothing!
    delete header;
    delete dataBytes;
}

uint8_t UDP4Datagram::copyTo(NetworkByteBlock *output) {
    if (
            dataBytes == nullptr ||
            output == nullptr ||
            dataBytes->getLength() > (size_t) (UDP4DATAPART_MAX_LENGTH - header->getSize()) ||
            dataBytes->getLength() == 0 ||
            header->getSize() > UDP4HEADER_MAX_LENGTH
            ) {
        return 1;
    }

    uint8_t errors = 0;
    errors += header->copyTo(output);

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Append dataBytes if no errors occurred yet
    output->append(dataBytes, dataBytes->getLength());
    return 0;
}

size_t UDP4Datagram::getLengthInBytes() {
    if(header == nullptr || dataBytes== nullptr){
        return 0;
    }
    return header->getSize() + dataBytes->getLength();
}

IP4DataPart::IP4ProtocolType UDP4Datagram::getIP4ProtocolType() {
    return IP4ProtocolType::UDP;
}

uint8_t UDP4Datagram::parseHeader(NetworkByteBlock *input) {
    if (
            input == nullptr ||
            header == nullptr ||
            input->bytesRemaining() < header->getSize()
            ) {
        return 1;
    }
    uint8_t errors = 0;
    errors += header->parse(input);

    return errors;
}