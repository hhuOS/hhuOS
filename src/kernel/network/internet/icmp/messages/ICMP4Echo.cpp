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
    auto *reply =
            new ICMP4EchoReply(
                    this->echoMessage.identifier,
                    this->echoMessage.sequenceNumber + (uint16_t )1
            );
    return reply;
}

ICMP4Message::ICMP4MessageType ICMP4Echo::getICMP4MessageType() {
    return ICMP4MessageType::ECHO;
}

uint8_t ICMP4Echo::copyTo(NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->append(echoMessage.type);
    errors += output->append(echoMessage.code);
    errors += output->append(echoMessage.checksum);
    errors += output->append(echoMessage.identifier);
    errors += output->append(echoMessage.sequenceNumber);

    return errors;
}

uint8_t ICMP4Echo::parseHeader(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() != sizeof echoMessage) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&echoMessage.type);
    errors += input->read(&echoMessage.code);
    errors += input->read(&echoMessage.checksum);
    errors += input->read(&echoMessage.identifier);
    errors += input->read(&echoMessage.sequenceNumber);

    return errors;
}