//
// Created by hannes on 17.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "UDP4Datagram.h"

//Private method!
uint8_t UDP4Datagram::do_copyTo(Kernel::NetworkByteBlock *output) {
    if (
            udp4Header == nullptr ||
            udp4DataBytes == nullptr ||
            udp4DataBytes->getLength() > (uint16_t) (UDP4DATAPART_MAX_LENGTH - UDP4Header::getHeaderLength())
            ) {
        return 1;
    }
    if (udp4Header->copyTo(output)) {
        return 1;
    }
    //Append dataBytes if no errors occurred yet
    return output->appendStraightFrom(udp4DataBytes, udp4DataBytes->getLength());
}

//Private method!
uint16_t UDP4Datagram::do_length() {
    if (udp4Header == nullptr) {
        return 0;
    }
    return udp4Header->getTotalDatagramLength();
}

//Private method!
IP4DataPart::IP4ProtocolType UDP4Datagram::do_getIP4ProtocolType() {
    return IP4ProtocolType::UDP;
}

UDP4Datagram::UDP4Datagram(uint16_t sourcePort, uint16_t destinationPort, Kernel::NetworkByteBlock *dataBytes) {
    this->udp4Header = new UDP4Header(sourcePort, destinationPort, dataBytes);
    this->udp4DataBytes = dataBytes;
}

UDP4Datagram::~UDP4Datagram() {
    //delete on nullptr simply does nothing!
    delete udp4Header;
    delete udp4DataBytes;
}
