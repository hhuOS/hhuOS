//
// Created by hannes on 17.05.21.
//

#include <kernel/network/internet/addressing/IP4Address.h>
#include "ICMP4EchoReply.h"

ICMP4EchoReply::ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber) {
    echoReply.type = 0; //8 for echo, 0 for echo reply (RFC792)
    echoReply.code = 0;
    echoReply.checksum = 0;
    echoReply.identifier = identifier;
    echoReply.sequenceNumber = sequenceNumber;
}

uint8_t ICMP4EchoReply::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->appendOneByte(echoReply.type);
    errors += output->appendOneByte(echoReply.code);
    errors += output->appendTwoBytesSwapped(echoReply.checksum);
    errors += output->appendTwoBytesSwapped(echoReply.identifier);
    errors += output->appendTwoBytesSwapped(echoReply.sequenceNumber);

    return errors;
}

size_t ICMP4EchoReply::getLengthInBytes() {
    return sizeof(echoReplyMessage);
}

ICMP4Message::ICMP4MessageType ICMP4EchoReply::getICMP4MessageType() {
    return ICMP4MessageType::ECHO_REPLY;
}

uint8_t ICMP4EchoReply::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() != sizeof echoReply) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->readOneByteTo(&echoReply.type);
    errors += input->readOneByteTo(&echoReply.code);
    errors += input->readTwoBytesSwappedTo(&echoReply.checksum);
    errors += input->readTwoBytesSwappedTo(&echoReply.identifier);
    errors += input->readTwoBytesSwappedTo(&echoReply.sequenceNumber);

    return errors;
}

IP4DataPart::IP4ProtocolType ICMP4EchoReply::getIP4ProtocolType() {
    return ICMP4Message::getIP4ProtocolType();
}

uint16_t ICMP4EchoReply::getIdentifier() const {
    return echoReply.identifier;
}

uint16_t ICMP4EchoReply::getSequenceNumber() const {
    return echoReply.sequenceNumber;
}
