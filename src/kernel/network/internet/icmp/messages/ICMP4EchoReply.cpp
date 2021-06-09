//
// Created by hannes on 17.05.21.
//

#include <kernel/network/internet/addressing/IP4Address.h>
#include <lib/libc/printf.h>
#include "ICMP4EchoReply.h"

ICMP4EchoReply::ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber) {
    echoReply.type = 0; //8 for echo, 0 for echo reply (RFC792)
    echoReply.code = 0;
    echoReply.checksum = 0;
    echoReply.identifier = identifier;
    echoReply.sequenceNumber = sequenceNumber;
}

ICMP4EchoReply::ICMP4EchoReply(IP4Address *sourceAddress) {
    sourceAddress->copyTo(ip4Info.sourceAddress);
}

uint8_t ICMP4EchoReply::copyTo(NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->append(echoReply.type);
    errors += output->append(echoReply.code);
    errors += output->append(echoReply.checksum);
    errors += output->append(echoReply.identifier);
    errors += output->append(echoReply.sequenceNumber);

    return errors;
}

size_t ICMP4EchoReply::getLengthInBytes() {
    return sizeof(echoReplyMessage);
}

ICMP4Message::ICMP4MessageType ICMP4EchoReply::getICMP4MessageType() {
    return ICMP4MessageType::ECHO_REPLY;
}

void ICMP4EchoReply::printAttributes() {
    printf("Echo reply from %d.%d.%d.%d received! Identifier: %d, SequenceNumber: %d",
           ip4Info.sourceAddress[0], ip4Info.sourceAddress[1], ip4Info.sourceAddress[2], ip4Info.sourceAddress[3],
           echoReply.identifier, echoReply.sequenceNumber
    );
}

uint8_t ICMP4EchoReply::parseHeader(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() != sizeof echoReply) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&echoReply.type);
    errors += input->read(&echoReply.code);
    errors += input->read(&echoReply.checksum);
    errors += input->read(&echoReply.identifier);
    errors += input->read(&echoReply.sequenceNumber);

    return errors;
}

IP4DataPart::IP4ProtocolType ICMP4EchoReply::getIP4ProtocolType() {
    return ICMP4Message::getIP4ProtocolType();
}
