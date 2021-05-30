//
// Created by hannes on 14.05.21.
//

#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    //header length is given in "lines" of 4 Bytes each
    //->header length in Bytes is value of headerLength field multiplied by 4
    this->headerLengthInBytes = (header.version_headerLength - 0x40) * 4;

    header.totalLength = headerLengthInBytes + ip4DataPart->getLengthInBytes();
    header.protocolType = ip4DataPart->getIP4ProtocolTypeAsInt();
    header.destinationAddress = destinationAddress->asInt();

    this->ip4DataPart = ip4DataPart;
}

IP4Datagram::IP4Datagram(EthernetDataPart *ethernetDataPart) {
//TODO: Implement parsing of parameters from given ethernetDataPart
}

IP4DataPart::IP4ProtocolType IP4Datagram::getIP4ProtocolType() const {
    return IP4DataPart::parseIntAsIP4ProtocolType(header.protocolType);
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return new IP4Address(header.sourceAddress);
}

void IP4Datagram::setSourceAddress(IP4Address *sourceAddress) {
    header.sourceAddress = sourceAddress->asInt();
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return new IP4Address(header.destinationAddress);
}

IP4DataPart *IP4Datagram::getIp4DataPart() const {
    return ip4DataPart;
}

uint8_t IP4Datagram::copyDataTo(NetworkByteBlock *byteBlock) {
    if (this->ip4DataPart == nullptr || byteBlock == nullptr) {
        return 1;
    }
    if (byteBlock->writeBytes(&this->header, sizeof(this->header))) {
        return 1;
    }
    return this->ip4DataPart->copyDataTo(byteBlock);
}

uint16_t IP4Datagram::getLengthInBytes() {
    return header.totalLength;
}

EthernetDataPart::EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}
