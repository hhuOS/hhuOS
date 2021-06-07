//
// Created by hannes on 17.05.21.
//

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

uint8_t ICMP4DestinationUnreachable::parse(NetworkByteBlock *input) {
    if (input == nullptr ||
        input->bytesRemaining() <= sizeof header
            ) {
        return 1;
    }
    //NOTE: The first Byte for 'type' is already read in IP4Datagram!
    //-> the next Byte in our NetworkByteBlock is 'code', the next value!
    //This is no problem here, because the 'type' value is constant '3' per definition
    uint8_t errors = 0;
    errors += input->read(&header.code);
    errors += input->read(&header.checksum);
    errors += input->read(&header.unused);
    if (errors) {
        return errors;
    }

    auto *ip4Datagram = new IP4Datagram();
    //This will stop with an error if datagram's body is greater than eight bytes
    //-> we will ignore this error here,
    // because the first eight bytes should be enough to identify the sending process
    ip4Datagram->parseHeader(input);

    //TODO: Implement further processing
    switch (ip4Datagram->getIP4DataPart()->getIP4ProtocolType()) {
        case IP4ProtocolType::ICMP4:
            break;
        case IP4ProtocolType::UDP:
            break;
        case IP4ProtocolType::INVALID:
            break;
    }
    return 0;
}