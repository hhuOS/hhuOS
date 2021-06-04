//
// Created by hannes on 14.05.21.
//

#include <kernel/event/network/ICMP4ReceiveEvent.h>
#include "IP4Datagram.h"

IP4Datagram::IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart) {
    this->destinationAddress=destinationAddress;
    this->ip4DataPart = ip4DataPart;
}

IP4DataPart::IP4ProtocolType IP4Datagram::getIP4ProtocolType() const {
    return ip4DataPart->getIP4ProtocolType();
}

IP4Address *IP4Datagram::getSourceAddress() const {
    return sourceAddress;
}

void IP4Datagram::setSourceAddress(IP4Address *source) {
    this->sourceAddress = source;
}

IP4Address *IP4Datagram::getDestinationAddress() const {
    return destinationAddress;
}

size_t IP4Datagram::getLengthInBytes() {
    return sizeof (header) + ip4DataPart->getLengthInBytes();
}

EthernetDataPart::EtherType IP4Datagram::getEtherType() {
    return EtherType::IP4;
}

uint8_t IP4Datagram::copyTo(NetworkByteBlock *output) {
    if (
        //if initialized with input byteBlock, this method must not continue
            ip4DataPart == nullptr ||
            destinationAddress == nullptr ||
            output == nullptr ||
            ip4DataPart->getLengthInBytes() > (size_t) (IP4DATAPART_MAX_LENGTH - sizeof(header)) ||
            sizeof (header) > IP4HEADER_MAX_LENGTH
            ) {
        return 1;
    }

    destinationAddress->copyTo(header.destinationAddress);
    sourceAddress->copyTo(header.sourceAddress);
    header.protocolType = ip4DataPart->getIP4ProtocolTypeAsInt();

    header.totalLength = getLengthInBytes();

    uint8_t errors = 0;
    errors+=output->append(header.version_headerLength);
    errors+=output->append(header.typeOfService);
    errors+=output->append(header.totalLength);
    errors+=output->append(header.identification);
    errors+=output->append(header.flags_fragmentOffset);
    errors+=output->append(header.timeToLive);
    errors+=output->append(header.protocolType);
    errors+=output->append(header.headerChecksum);
    errors+=output->append(header.sourceAddress, IP4ADDRESS_LENGH);
    errors+=output->append(header.destinationAddress, IP4ADDRESS_LENGH);
    errors+=ip4DataPart->copyDataTo(output);

    return errors;
}

uint8_t IP4Datagram::parse(NetworkByteBlock *input) {
    if (input == nullptr) {
        return 1;
    }
    uint8_t errors = 0;
    errors+=input->read(&header.version_headerLength);
    errors+=input->read(&header.typeOfService);
    errors+=input->read(&header.totalLength);
    errors+=input->read(&header.identification);
    errors+=input->read(&header.flags_fragmentOffset);
    errors+=input->read(&header.timeToLive);
    errors+=input->read(&header.protocolType);
    errors+=input->read(&header.headerChecksum);
    errors+=input->read(header.sourceAddress, IP4ADDRESS_LENGH);
    errors+=input->read(header.destinationAddress, IP4ADDRESS_LENGH);

    switch (IP4DataPart::parseIntAsIP4ProtocolType(header.protocolType)) {
        case IP4DataPart::IP4ProtocolType::ICMP4:{
            this->ip4DataPart = new GenericICMP4Message(input);
            errors+=ip4DataPart->parseInput();
            break;
        }
        case IP4DataPart::IP4ProtocolType::UDP:{
            this->ip4DataPart = new UDPDatagram(input);
            errors+=ip4DataPart->parseInput();
            break;
        }
        case IP4DataPart::IP4ProtocolType::INVALID:
            break;
    }

    return errors;
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

IP4Datagram::~IP4Datagram() {
    delete destinationAddress;
    delete sourceAddress;
    ip4DataPart->freeMemory();
}
