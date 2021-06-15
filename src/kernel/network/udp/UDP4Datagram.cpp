//
// Created by hannes on 17.05.21.
//

#include <lib/libc/printf.h>
#include "UDP4Datagram.h"

UDP4Datagram::UDP4Datagram(UDP4Port *sourcePort, UDP4Port *destinationPort, uint8_t *outgoingBytes, size_t dataLength) {
    this->dataBytes = new NetworkByteBlock(dataLength);
    this->dataBytes->append(outgoingBytes, dataLength);
    this->dataBytes->resetIndex();

    this->header = new UDP4Header(sourcePort, destinationPort, this->dataBytes);
}

UDP4Datagram::~UDP4Datagram() {
    //delete on nullptr simply does nothing!
    delete header;
    delete dataBytes;
}

uint8_t UDP4Datagram::copyTo(NetworkByteBlock *output) {
    if (
            header == nullptr ||
            dataBytes == nullptr ||
            output == nullptr ||
            dataBytes->getLength() > (size_t) (UDP4DATAPART_MAX_LENGTH - header->getHeaderSize()) ||
            dataBytes->getLength() == 0 ||
            header->getHeaderSize() > UDP4HEADER_MAX_LENGTH
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
    return output->append(dataBytes, dataBytes->getLength());
}

size_t UDP4Datagram::getLengthInBytes() {
    if (header == nullptr) {
        return 0;
    }
    return header->getDatagramLength();
}

IP4DataPart::IP4ProtocolType UDP4Datagram::getIP4ProtocolType() {
    return IP4ProtocolType::UDP;
}