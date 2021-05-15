//
// Created by hannes on 14.05.21.
//

#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(uint8_t protocol, IP4Address *destinationAddress, NetworkDataPart *dataPart) {
    this->protocol = protocol;
    this->dataPart = dataPart;
    this->destinationAddress = destinationAddress;
}

uint8_t IP4Datagram::getProtocol() const {
    return protocol;
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return destinationAddress;
}

NetworkDataPart *IP4Datagram::getDataPart() const {
    return dataPart;
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return sourceAddress;
}

void IP4Datagram::setSourceAddress(IP4Address *sourceAddress) {
    IP4Datagram::sourceAddress = sourceAddress;
}

void IP4Datagram::setDestinationAddress(IP4Address *destinationAddress) {
    IP4Datagram::destinationAddress = destinationAddress;
}

void IP4Datagram::setDataPart(NetworkDataPart *dataPart) {
    IP4Datagram::dataPart = dataPart;
}

void *IP4Datagram::getData() {
    return nullptr;
}

uint16_t IP4Datagram::getLength() {
    return 0;
}
