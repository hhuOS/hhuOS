//
// Created by hannes on 14.05.21.
//

#include <kernel/network/internet/icmp/ICMP4EchoReply.h>
#include <kernel/network/internet/icmp/ICMP4Echo.h>
#include <kernel/network/NetworkDefinitions.h>
#include <kernel/network/udp/UDP4Datagram.h>
#include "IP4Datagram.h"

//Private method!
uint8_t IP4Datagram::do_copyTo(Kernel::NetworkByteBlock *output) {
    if (
            header == nullptr ||
            ip4DataPart == nullptr ||
            ip4DataPart->getLengthInBytes() > (size_t) (IP4DATAPART_MAX_LENGTH - header->getHeaderLength()) ||
            header->getHeaderLength() > IP4HEADER_MAX_LENGTH //IPv4 headers are dynamical!
            ) {
        return 1;
    }
    if (header->copyTo(output)) {
        return 1;
    }
    //Call next level if no errors occurred yet
    return ip4DataPart->copyTo(output);
}

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    this->header = new IP4Header(destinationAddress, ip4DataPart);
    this->ip4DataPart = ip4DataPart;
}

IP4Datagram::~IP4Datagram() {
    //delete on nullptr simply does nothing!
    delete header;
    delete ip4DataPart;
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    if (header == nullptr) {
        return nullptr;
    }
    return header->getDestinationAddress();
}

uint8_t IP4Datagram::setSourceAddress(IP4Address *source) {
    if (source == nullptr) {
        return 1;
    }
    return header->setSourceAddress(source);
}

size_t IP4Datagram::getLengthInBytes() {
    if (header == nullptr) {
        return 0;
    }
    return header->getTotalDatagramLength();
}

EthernetDataPart::EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}

uint8_t IP4Datagram::fillHeaderChecksum() {
    if (header == nullptr) {
        return 1;
    }
    return header->fillChecksumField();
}

String IP4Datagram::asString(String spacing) {
    return spacing + "[IP4Datagram]";
}
