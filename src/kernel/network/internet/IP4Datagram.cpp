//
// Created by hannes on 14.05.21.
//

#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    //header length is given in "lines" of 4 Bytes each
    //-> total size in Bytes is (header length)*4 + length of data part in Bytes
    uint8_t headerLength = (header.version_headerLength - 0x40);
    header.totalLength =  headerLength* 4 + ip4DataPart->getLengthInBytes();

    uint8_t protocolTypeInt=ip4DataPart->getIP4ProtocolTypeAsInt();
    memcpy(&header.protocolType,&protocolTypeInt,1);

    header.destinationAddress = destinationAddress->asInt();
}

IP4Datagram::IP4Datagram(EthernetDataPart *ethernetDataPart) {
//TODO: Implement parsing of parameters from given ethernetDataPart
}

IP4DataPart::IP4ProtocolType IP4Datagram::getIP4ProtocolType() const {
    return IP4DataPart::parseInt(header.protocolType);
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return new IP4Address(header.sourceAddress);
}

void IP4Datagram::setSourceAddress(IP4Address *sourceAddress) {
    header.sourceAddress=sourceAddress->asInt();
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return new IP4Address(header.destinationAddress);
}

IP4DataPart *IP4Datagram::getIp4DataPart() const {
    return ip4DataPart;
}

void *IP4Datagram::getMemoryAddress() {
    return nullptr;
}

uint16_t IP4Datagram::getLengthInBytes() {
    return header.totalLength;
}

EthernetDataPart::EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}
