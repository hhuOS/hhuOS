//
// Created by hannes on 14.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "ICMP4Echo.h"

ICMP4Echo::ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber) {
    echoMessage.checksum = 0; //TODO: Implement checksum calculation
    echoMessage.identifier = identifier;
    echoMessage.sequenceNumber = sequenceNumber;
}

size_t ICMP4Echo::getLengthInBytes() {
    return sizeof echoMessage;
}

ICMP4EchoReply *ICMP4Echo::buildEchoReply() const {
    return new ICMP4EchoReply(
            this->echoMessage.identifier,
            this->echoMessage.sequenceNumber
    );
}

ICMP4Message::ICMP4MessageType ICMP4Echo::getICMP4MessageType() {
    return ICMP4MessageType::ECHO;
}

uint8_t ICMP4Echo::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->appendOneByte(echoMessage.type);
    errors += output->appendOneByte(echoMessage.code);
    errors += output->appendTwoBytesSwapped(echoMessage.checksum);
    errors += output->appendTwoBytesSwapped(echoMessage.identifier);
    errors += output->appendTwoBytesSwapped(echoMessage.sequenceNumber);

    return errors;
}

uint8_t ICMP4Echo::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() != sizeof echoMessage) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->readOneByteTo(&echoMessage.type);
    errors += input->readOneByteTo(&echoMessage.code);
    errors += input->readTwoBytesSwappedTo(&echoMessage.checksum);
    errors += input->readTwoBytesSwappedTo(&echoMessage.identifier);
    errors += input->readTwoBytesSwappedTo(&echoMessage.sequenceNumber);

    return errors;
}