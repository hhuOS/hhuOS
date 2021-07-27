//
// Created by hannes on 14.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "IP4Datagram.h"

//Private method!
uint8_t IP4Datagram::do_copyTo(Kernel::NetworkByteBlock *output) {
    if (
            ip4Header == nullptr ||
            ip4DataPart == nullptr ||
            ip4DataPart->length() > (uint16_t) (IP4DATAPART_MAX_LENGTH - ip4Header->getHeaderLength()) ||
            ip4Header->getHeaderLength() > IP4HEADER_MAX_LENGTH //IPv4 headers are dynamical!
            ) {
        return 1;
    }
    if (ip4Header->copyTo(output)) {
        return 1;
    }
    //Call next level if no errors occurred yet
    return ip4DataPart->copyTo(output);
}

//Private method!
uint16_t IP4Datagram::do_length() {
    if (ip4Header == nullptr) {
        return 0;
    }
    return ip4Header->getTotalDatagramLength();
}

//Private method!
EthernetDataPart::EtherType IP4Datagram::do_getEtherType() {
    return EtherType::IP4;
}

//Private method!
IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    this->ip4Header = new IP4Header(destinationAddress, ip4DataPart);
    this->ip4DataPart = ip4DataPart;
}

IP4Datagram::~IP4Datagram() {
    //delete on nullptr simply does nothing!
    delete ip4Header;
    delete ip4DataPart;
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    if (ip4Header == nullptr) {
        return nullptr;
    }
    return ip4Header->getDestinationAddress();
}

uint8_t IP4Datagram::setSourceAddress(IP4Address *source) {
    if (source == nullptr) {
        return 1;
    }
    return ip4Header->setSourceAddress(source);
}

uint8_t IP4Datagram::fillHeaderChecksum() {
    if (ip4Header == nullptr) {
        return 1;
    }
    return ip4Header->fillChecksumField();
}
