//
// Created by hannes on 17.05.21.
//

#include <lib/libc/printf.h>
#include <kernel/network/NetworkDefinitions.h>
#include "UDP4Datagram.h"

UDP4Datagram::UDP4Datagram(uint16_t sourcePort, uint16_t destinationPort, Kernel::NetworkByteBlock *dataBytes) {
    this->header = new UDP4Header(sourcePort, destinationPort, dataBytes);
    this->dataBytes = dataBytes;
}

UDP4Datagram::~UDP4Datagram() {
    //delete on nullptr simply does nothing!
    delete header;
    delete dataBytes;
}

uint8_t UDP4Datagram::copyTo(Kernel::NetworkByteBlock *output) {
    if (
            header == nullptr ||
            dataBytes == nullptr ||
            output == nullptr ||
            dataBytes->getLength() > (size_t) (UDP4DATAPART_MAX_LENGTH - UDP4Header::getHeaderLength()) ||
            dataBytes->getLength() == 0 ||
                    UDP4Header::getHeaderLength() > UDP4HEADER_MAX_LENGTH
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
    return header->getTotalDatagramLength();
}

IP4DataPart::IP4ProtocolType UDP4Datagram::getIP4ProtocolType() {
    return IP4ProtocolType::UDP;
}