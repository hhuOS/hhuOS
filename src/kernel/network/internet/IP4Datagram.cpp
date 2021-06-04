//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    //header length is given in "lines" of 4 Bytes each
    //->header length in Bytes is value of headerLength field multiplied by 4
    headerLengthInBytes = (header.version_headerLength - 0x40) * 4;

    header.totalLength = headerLengthInBytes + ip4DataPart->getLengthInBytes();
    header.protocolType = ip4DataPart->getIP4ProtocolTypeAsInt();
    destinationAddress->copyTo(header.destinationAddress);

    this->ip4DataPart = ip4DataPart;
}

IP4Datagram::IP4Datagram(NetworkByteBlock *input) {
    this->input = input;
}

IP4Datagram::~IP4Datagram() {
    delete input;
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
            ip4DataPart == nullptr ||
            byteBlock == nullptr ||
            ip4DataPart->getLengthInBytes() > (size_t) (IP4DATAPART_MAX_LENGTH - headerLengthInBytes) ||
            headerLengthInBytes > IP4HEADER_MAX_LENGTH
            ) {
        return 1;
    }
    uint8_t retVals = 0;
    retVals+=byteBlock->append(header.version_headerLength);
    retVals+=byteBlock->append(header.typeOfService);
    retVals+=byteBlock->append(header.totalLength);
    retVals+=byteBlock->append(header.identification);
    retVals+=byteBlock->append(header.flags_fragmentOffset);
    retVals+=byteBlock->append(header.timeToLive);
    retVals+=byteBlock->append(header.protocolType);
    retVals+=byteBlock->append(header.headerChecksum);
    retVals+=byteBlock->append(header.sourceAddress, IP4ADDRESS_LENGH);
    retVals+=byteBlock->append(header.destinationAddress, IP4ADDRESS_LENGH);
    retVals+=ip4DataPart->copyDataTo(byteBlock);

    return retVals;
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
    uint8_t retVals = 0;
    retVals+=input->read(&header.version_headerLength);
    retVals+=input->read(&header.typeOfService);
    retVals+=input->read(&header.totalLength);
    retVals+=input->read(&header.identification);
    retVals+=input->read(&header.flags_fragmentOffset);
    retVals+=input->read(&header.timeToLive);
    retVals+=input->read(&header.protocolType);
    retVals+=input->read(&header.headerChecksum);
    retVals+=input->read(header.sourceAddress, IP4ADDRESS_LENGH);
    retVals+=input->read(header.destinationAddress, IP4ADDRESS_LENGH);

    return retVals;
}

GenericICMP4Message *IP4Datagram::buildGenericICMP4MessageWithInput() {
    return new GenericICMP4Message(
            new IP4Address(header.destinationAddress),
            new IP4Address(header.sourceAddress),
            input
    );
}

UDPDatagram *IP4Datagram::buildUDPDatagramWithInput() {
    return new UDPDatagram(input);
}
