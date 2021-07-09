//
// Created by hannes on 15.06.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include "IP4Header.h"

uint8_t IP4Header::calculateInternetChecksum(uint16_t *target, Kernel::NetworkByteBlock *content) {
    if (target == nullptr) {
        return 1;
    }

    uint16_t contentLength = content->getLength();
    if ((contentLength % 2) != 0) {
        return 1;
    }

    uint16_t tempValue = 0;
    uint32_t result = 0;

    //We read two bytes at once!
    contentLength = contentLength / (uint16_t) 2;

    for (uint16_t i = 0; i < contentLength; i++) {
        if (content->readTwoBytesSwappedTo(&tempValue)) {
            return 1;
        }
        result += tempValue;
    }
    //add overflow bits from other two bytes
    result += ((uint16_t *) &result)[1];

    //read only first two bytes from four byte integer as result
    tempValue = (uint16_t) result;
    *target = ~tempValue;
    return 0;
}

IP4Header::IP4Header(IP4Address *destinationAddress, IP4DataPart *dataPart) {
    this->destinationAddress = destinationAddress;
    protocolType = dataPart->getIP4ProtocolType();

    //we use minimal header if we create one
    totalLength = (uint16_t) IP4HEADER_MIN_LENGTH + (uint16_t) dataPart->getLengthInBytes();
}

IP4Header::~IP4Header() {
    delete sourceAddress;
    delete destinationAddress;
}

uint16_t IP4Header::getTotalDatagramLength() const {
    return totalLength;
}

IP4DataPart::IP4ProtocolType IP4Header::getIP4ProtocolType() const {
    return protocolType;
}

IP4Address *IP4Header::getDestinationAddress() {
    return destinationAddress;
}

IP4Address *IP4Header::getSourceAddress() {
    return sourceAddress;
}

uint8_t IP4Header::setSourceAddress(IP4Address *address) {
    if (address == nullptr) {
        return 1;
    }
    //Cleanup if already set
    delete this->sourceAddress;
    this->sourceAddress = address;
    return 0;
}

uint16_t IP4Header::getHeaderLength() const {
    //IP4 header length is not fixed size
    //-> calculate it from real header value for header length!
    return static_cast<uint16_t>((version_headerLength - 0x40) * 4);
}

uint8_t IP4Header::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->appendOneByte(version_headerLength);
    errors += output->appendOneByte(typeOfService);
    errors += output->appendTwoBytesSwapped(totalLength);
    errors += output->appendTwoBytesSwapped(identification);
    errors += output->appendTwoBytesSwapped(flags_fragmentOffset);
    errors += output->appendOneByte(timeToLive);
    errors += output->appendOneByte((uint8_t) protocolType);
    errors += output->appendTwoBytesSwapped(headerChecksum);

    if (errors) {
        return errors;
    }

    uint8_t addressBytes[IP4ADDRESS_LENGTH];
    sourceAddress->copyTo(addressBytes);
    errors += output->appendStraightFrom(addressBytes, IP4ADDRESS_LENGTH);

    if (errors) {
        return errors;
    }

    destinationAddress->copyTo(addressBytes);
    errors += output->appendStraightFrom(addressBytes, IP4ADDRESS_LENGTH);
    return errors;
}

uint8_t IP4Header::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < IP4HEADER_MIN_LENGTH) {
        return 1;
    }

    if (sourceAddress != nullptr || destinationAddress != nullptr) {
        //Stop if already initialized!
        //-> no existing data is overwritten
        return 1;
    }

    uint8_t errors = 0;
    errors += input->readOneByteTo(&version_headerLength);
    errors += input->readOneByteTo(&typeOfService);
    errors += input->readTwoBytesSwappedTo(&totalLength);
    errors += input->readTwoBytesSwappedTo(&identification);
    errors += input->readTwoBytesSwappedTo(&flags_fragmentOffset);
    errors += input->readOneByteTo(&timeToLive);

    uint8_t typeValue = 0;
    errors += input->readOneByteTo(&typeValue);
    protocolType = IP4DataPart::parseIntAsIP4ProtocolType(typeValue);

    errors += input->readTwoBytesSwappedTo(&headerChecksum);

    if (errors) {
        return errors;
    }

    uint8_t addressBytes[IP4ADDRESS_LENGTH];

    if (input->readStraightTo(addressBytes, IP4ADDRESS_LENGTH)) {
        return 1;
    }
    sourceAddress = new IP4Address(addressBytes);

    if (input->readStraightTo(addressBytes, IP4ADDRESS_LENGTH)) {
        return 1;
    }
    destinationAddress = new IP4Address(addressBytes);

    return errors;
}

bool IP4Header::headerIsValid() {
    if (headerChecksum == 0) {
        //Header checksum not parsed!
        return false;
    }

    uint16_t calculationResult = 0;
    auto *headerAsBytes = new Kernel::NetworkByteBlock(getHeaderLength());
    if (this->copyTo(headerAsBytes)) {
        delete headerAsBytes;
        return false;
    }
    headerAsBytes->resetIndex();

    if (calculateInternetChecksum(&calculationResult, headerAsBytes)) {
        delete headerAsBytes;
        return false;
    }

    delete headerAsBytes;
    return calculationResult == 0;
}

uint8_t IP4Header::fillChecksumField() {
    if (headerChecksum != 0) {
        //Header checksum already set!
        return 1;
    }

    auto *headerAsBytes = new Kernel::NetworkByteBlock(getHeaderLength());
    if (this->copyTo(headerAsBytes)) {
        delete headerAsBytes;
        return 1;
    }
    headerAsBytes->resetIndex();

    uint16_t calculationResult = 0;
    if (calculateInternetChecksum(&calculationResult, headerAsBytes)) {
        delete headerAsBytes;
        return 1;
    }
    headerChecksum = calculationResult;
    delete headerAsBytes;
    return 0;
}

bool IP4Header::destinationIs(IP4Address *otherAddress) {
    if (otherAddress == nullptr || destinationAddress == nullptr) {
        return false;
    }
    return destinationAddress->equals(otherAddress);
}

bool IP4Header::hasOptionsOrFragmentation() const {
    return
            getHeaderLength() > IP4HEADER_MIN_LENGTH || //OPTIONS field ist behind regular fields
            (flags_fragmentOffset + identification) > 0;
}
