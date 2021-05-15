//
// Created by hannes on 14.05.21.
//

#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, NetworkDataPart *dataPart) {
    this->destinationAddress=destinationAddress;
    this->dataPart=dataPart;
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
