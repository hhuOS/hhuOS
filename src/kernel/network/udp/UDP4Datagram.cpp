//
// Created by hannes on 17.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "UDP4Datagram.h"

//Private method!
uint8_t UDP4Datagram::do_copyTo(Kernel::NetworkByteBlock *output) {
    if (
            header == nullptr ||
            dataBytes == nullptr ||
            dataBytes->getLength() > (size_t) (UDP4DATAPART_MAX_LENGTH - UDP4Header::getHeaderLength())
            ) {
        return 1;
    }
    if (header->copyTo(output)) {
        return 1;
    }
    //Append dataBytes if no errors occurred yet
    return output->appendStraightFrom(dataBytes, dataBytes->getLength());
}

UDP4Datagram::UDP4Datagram(uint16_t sourcePort, uint16_t destinationPort, Kernel::NetworkByteBlock *dataBytes) {
    this->header = new UDP4Header(sourcePort, destinationPort, dataBytes);
    this->dataBytes = dataBytes;
}

UDP4Datagram::~UDP4Datagram() {
    //delete on nullptr simply does nothing!
    delete header;
    delete dataBytes;
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