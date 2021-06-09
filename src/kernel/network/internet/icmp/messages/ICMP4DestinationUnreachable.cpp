//
// Created by hannes on 17.05.21.
//

#include <lib/libc/printf.h>
#include "ICMP4DestinationUnreachable.h"

ICMP4DestinationUnreachable::ICMP4DestinationUnreachable(size_t ip4HeaderSize) {
    if (ip4HeaderSize > IP4HEADER_MAX_LENGTH) {
        ip4HeaderSize = IP4HEADER_MAX_LENGTH;
    }
    this->internalBytes = new NetworkByteBlock(sizeof this->header + ip4HeaderSize + 8);
}

ICMP4DestinationUnreachable::ICMP4DestinationUnreachable(uint8_t errorCode, IP4Datagram *datagram) {
    header.code = errorCode;

    this->internalBytes = new NetworkByteBlock(datagram->getHeaderLengthInBytes() + 8);

    //copy header and first 8 bytes to byteBlock
    //this will stop with an error if datagram's body has more than eight byte
    //-> we simply ignore this error here, for details see RFC792 page 4
    datagram->copyTo(internalBytes);
}

ICMP4DestinationUnreachable::~ICMP4DestinationUnreachable() {
    delete internalBytes;
}

uint8_t ICMP4DestinationUnreachable::copyTo(NetworkByteBlock *byteBlock) {
    if (byteBlock == nullptr || internalBytes == nullptr) {
        return 1;
    }
    uint8_t errors = 0;
    errors += byteBlock->append(header.type);
    errors += byteBlock->append(header.code);
    errors += byteBlock->append(header.checksum);
    errors += byteBlock->append(header.unused);
    if (errors) {
        return errors;
    }
    return byteBlock->append(this->internalBytes, this->internalBytes->getLength());
}

size_t ICMP4DestinationUnreachable::getLengthInBytes() {
    if (internalBytes == nullptr) {
        return sizeof header;
    }
    return sizeof header + internalBytes->getLength();
}

ICMP4Message::ICMP4MessageType ICMP4DestinationUnreachable::getICMP4MessageType() {
    return ICMP4MessageType::DESTINATION_UNREACHABLE;
}

uint8_t ICMP4DestinationUnreachable::parseHeader(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() != (sizeof header + 8)) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&header.type);
    errors += input->read(&header.code);
    errors += input->read(&header.checksum);
    errors += input->read(&header.unused);
    if (errors) {
        return errors;
    }

    return 0;
}

uint8_t ICMP4DestinationUnreachable::parseDataBytes(NetworkByteBlock *input) {
    internalBytes->append(input, input->getLength());
    return 0;
}

void ICMP4DestinationUnreachable::collectDatagramAttributes(Util::ArrayList<String> *strings) {
    auto *datagram = new IP4Datagram();
    datagram->parseHeader(internalBytes);
    printf("Destination unreachable!\n");
//    datagram->printHeaderInformation(); //TODO: Implement this one!
//    internalBytes->printRemainingBytes();
    delete datagram;
}
