//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    //header length is given in "lines" of 4 Bytes each
    //->header length in Bytes is value of headerLength field multiplied by 4
    this->headerLengthInBytes = (header.version_headerLength - 0x40) * 4;

    header.totalLength = headerLengthInBytes + ip4DataPart->getLengthInBytes();
    header.protocolType = ip4DataPart->getIP4ProtocolTypeAsInt();
    destinationAddress->copyTo(header.destinationAddress);

    this->ip4DataPart = ip4DataPart;
}

IP4Datagram::IP4Datagram(NetworkByteBlock *input) {
    this->input = input;
}

IP4Datagram::~IP4Datagram() {
    delete this->input;
}

IP4DataPart::IP4ProtocolType IP4Datagram::getIP4ProtocolType() const {
    return IP4DataPart::parseIntAsIP4ProtocolType(header.protocolType);
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return new IP4Address(header.sourceAddress);
}

void IP4Datagram::setSourceAddress(IP4Address *sourceAddress) {
    sourceAddress->copyTo(header.sourceAddress);
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return new IP4Address(header.destinationAddress);
}

uint8_t IP4Datagram::copyDataTo(NetworkByteBlock *byteBlock) {
    if (
        //if initialized with input byteBlock, this method must not continue
            this->ip4DataPart == nullptr ||
            byteBlock == nullptr ||
            this->ip4DataPart->getLengthInBytes() > (size_t)(IP4DATAPART_MAX_LENGTH - this->headerLengthInBytes) ||
            this->headerLengthInBytes > IP4HEADER_MAX_LENGTH
            ) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
            &this->header.version_headerLength,
            sizeof(this->header.version_headerLength))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
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
    if (byteBlock->appendBytesStraight(
            &this->header.timeToLive,
            sizeof(this->header.timeToLive))
            ) {
        return 1;
    }
    if (byteBlock->appendBytesStraight(
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

uint8_t IP4Datagram::parseInput() {
    if (input == nullptr) {
        return 1;
    }
    if (input->writeBytesStraightTo(
            &this->header.version_headerLength,
            sizeof(this->header.version_headerLength))
            ) {
        return 1;
    }
    if (input->writeBytesStraightTo(
            &this->header.typeOfService,
            sizeof(this->header.typeOfService))
            ) {
        return 1;
    }
    if (input->writeBytesInHostByteOrderTo(
            &this->header.totalLength,
            sizeof(this->header.totalLength))
            ) {
        return 1;
    }
    if (input->writeBytesInHostByteOrderTo(
            &this->header.identification,
            sizeof(this->header.identification))
            ) {
        return 1;
    }
    if (input->writeBytesInHostByteOrderTo(
            &this->header.flags_fragmentOffset,
            sizeof(this->header.flags_fragmentOffset))
            ) {
        return 1;
    }
    if (input->writeBytesStraightTo(
            &this->header.timeToLive,
            sizeof(this->header.timeToLive))
            ) {
        return 1;
    }
    if (input->writeBytesStraightTo(
            &this->header.protocolType,
            sizeof(this->header.protocolType))
            ) {
        return 1;
    }
    if (input->writeBytesInHostByteOrderTo(
            &this->header.headerChecksum,
            sizeof(this->header.headerChecksum))
            ) {
        return 1;
    }
    if (input->writeBytesStraightTo(
            &this->header.sourceAddress,
            sizeof(this->header.sourceAddress))
            ) {
        return 1;
    }
    if (input->writeBytesStraightTo(
            &this->header.destinationAddress,
            sizeof(this->header.destinationAddress))
            ) {
        return 1;
    }
    return 0;
}

GenericICMP4Message *IP4Datagram::buildGenericICMP4MessageWithInput() {
    return new GenericICMP4Message(
            new IP4Address(this->header.destinationAddress),
            new IP4Address(this->header.sourceAddress),
            this->input
    );
}

UDPDatagram *IP4Datagram::buildUDPDatagramWithInput() {
    return new UDPDatagram(this->input);
}
