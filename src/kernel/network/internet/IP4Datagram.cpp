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
    destinationAddress->copyTo(&header.destinationAddress);

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
    sourceAddress->copyTo(&header.sourceAddress);
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
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->header.version_headerLength,
            sizeof(this->header.version_headerLength))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->header.typeOfService,
            sizeof(this->header.typeOfService))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->header.totalLength,
            sizeof(this->header.totalLength))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->header.identification,
            sizeof(this->header.identification))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->header.flags_fragmentOffset,
            sizeof(this->header.flags_fragmentOffset))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->header.timeToLive,
            sizeof(this->header.timeToLive))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->header.protocolType,
            sizeof(this->header.protocolType))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesInNetworkByteOrder(
            &this->header.headerChecksum,
            sizeof(this->header.headerChecksum))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->header.sourceAddress,
            sizeof(this->header.sourceAddress))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->header.destinationAddress,
            sizeof(this->header.destinationAddress))
            ) {
        return 1;
    }
    return this->ip4DataPart->copyDataTo(byteBlock);
}

size_t IP4Datagram::getLengthInBytes() {
    return header.totalLength;
}

EthernetDataPart::EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}
