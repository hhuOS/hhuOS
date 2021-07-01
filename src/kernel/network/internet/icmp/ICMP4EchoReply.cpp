//
// Created by hannes on 17.05.21.
//

#include <kernel/network/internet/IP4Address.h>
#include "ICMP4EchoReply.h"

ICMP4EchoReply::ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber) {
    header.type = 0; //8 for echo, 0 for echo reply (RFC792)
    header.code = 0;
    header.checksum = 0;
    echoReply.identifier = identifier;
    echoReply.sequenceNumber = sequenceNumber;
}

uint8_t ICMP4EchoReply::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->appendOneByte(header.type);
    errors += output->appendOneByte(header.code);
    errors += output->appendTwoBytesSwapped(header.checksum);
    errors += output->appendTwoBytesSwapped(echoReply.identifier);
    errors += output->appendTwoBytesSwapped(echoReply.sequenceNumber);

    return errors;
}

size_t ICMP4EchoReply::getLengthInBytes() {
    return sizeof(header) + sizeof(echoReply);
}

ICMP4Message::ICMP4MessageType ICMP4EchoReply::getICMP4MessageType() {
    return ICMP4MessageType::ECHO_REPLY;
}

uint8_t ICMP4EchoReply::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < (sizeof header + sizeof echoReply)) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->readOneByteTo(&header.type);
    errors += input->readOneByteTo(&header.code);
    errors += input->readTwoBytesSwappedTo(&header.checksum);
    errors += input->readTwoBytesSwappedTo(&echoReply.identifier);
    errors += input->readTwoBytesSwappedTo(&echoReply.sequenceNumber);

    return errors;
}

uint16_t ICMP4EchoReply::getIdentifier() const {
    return echoReply.identifier;
}

uint16_t ICMP4EchoReply::getSequenceNumber() const {
    return echoReply.sequenceNumber;
}
