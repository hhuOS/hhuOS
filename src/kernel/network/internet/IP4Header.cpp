//
// Created by hannes on 15.06.21.
//

#include "IP4Header.h"

IP4Header::IP4Header(IP4Address *destinationAddress, IP4DataPart *dataPart) {
    header.totalLength = sizeof header + dataPart->getLengthInBytes();
    header.protocolType = dataPart->getIP4ProtocolTypeAsInt();

    destinationAddress->copyTo(header.destinationAddress);
    this->destinationAddress = new IP4Address(header.destinationAddress);
}

IP4Header::~IP4Header() {
    delete sourceAddress;
    delete destinationAddress;
}

size_t IP4Header::getTotalLength() const {
    return (size_t) header.totalLength;
}

IP4DataPart::IP4ProtocolType IP4Header::getIP4ProtocolType() const {
    return IP4DataPart::parseIntAsIP4ProtocolType(header.protocolType);
}

IP4Address *IP4Header::getDestinationAddress() {
    return destinationAddress;
}

IP4Address *IP4Header::getSourceAddress() {
    return sourceAddress;
}

void IP4Header::setSourceAddress(IP4Address *address) {
    address->copyTo(header.sourceAddress);
    this->sourceAddress = new IP4Address(header.sourceAddress);
}

size_t IP4Header::getSize() const {
    //IP4 header length is not fixed size
    //-> calculate it from real header value for header length!
    return (size_t) (header.version_headerLength - 0x40) * 4;
}

uint8_t IP4Header::copyTo(NetworkByteBlock *output) {
    uint8_t errors = 0;
    errors += output->append(header.version_headerLength);
    errors += output->append(header.typeOfService);
    errors += output->append(header.totalLength);
    errors += output->append(header.identification);
    errors += output->append(header.flags_fragmentOffset);
    errors += output->append(header.timeToLive);
    errors += output->append(header.protocolType);
    errors += output->append(header.headerChecksum);
    errors += output->append(header.sourceAddress, IP4ADDRESS_LENGTH);
    errors += output->append(header.destinationAddress, IP4ADDRESS_LENGTH);
    return errors;
}

uint8_t IP4Header::parse(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < sizeof this->header) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&header.version_headerLength);
    errors += input->read(&header.typeOfService);
    errors += input->read(&header.totalLength);
    errors += input->read(&header.identification);
    errors += input->read(&header.flags_fragmentOffset);
    errors += input->read(&header.timeToLive);
    errors += input->read(&header.protocolType);
    errors += input->read(&header.headerChecksum);
    errors += input->read(header.sourceAddress, IP4ADDRESS_LENGTH);
    errors += input->read(header.destinationAddress, IP4ADDRESS_LENGTH);

    //Skip additional bytes if incoming header is larger than our internal one
    //-> next layer would read our remaining header bytes as data otherwise!
    size_t remainingHeaderBytes = getSize() - sizeof this->header;
    //True if remainingHeaderBytes > 0
    if (remainingHeaderBytes) {
        errors += input->skip(remainingHeaderBytes);
    }
    if (sourceAddress != nullptr) {
        //already initialized!
        errors++;
        return errors;
    }
    this->sourceAddress = new IP4Address(header.sourceAddress);
    return errors;
}

bool IP4Header::headerValid() {
    return calculateChecksum() == header.headerChecksum;
}

uint16_t IP4Header::calculateChecksum() {
    //TODO: Implement this one!
    return header.headerChecksum;
}