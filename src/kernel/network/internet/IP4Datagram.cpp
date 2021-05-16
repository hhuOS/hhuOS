//
// Created by hannes on 14.05.21.
//

#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    this->ip4ProtocolType=ip4DataPart->getIP4ProtocolType();
    this->destinationAddress=destinationAddress;
    this->ip4DataPart=ip4DataPart;
}

IP4Datagram::IP4Datagram(EthernetDataPart *ethernetDataPart) {
//TODO: Implement parsing of parameters from given ethernetDataPart
}

IP4ProtocolType IP4Datagram::getIp4ProtocolType() const {
    return ip4ProtocolType;
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return sourceAddress;
}

void IP4Datagram::setSourceAddress(IP4Address *sourceAddress) {
    IP4Datagram::sourceAddress = sourceAddress;
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return destinationAddress;
}

IP4DataPart *IP4Datagram::getIp4DataPart() const {
    return ip4DataPart;
}

void *IP4Datagram::getMemoryAddress() {
    return nullptr;
}

uint16_t IP4Datagram::getLength() {
    return 0;
}

EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}
