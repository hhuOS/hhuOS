//
// Created by hannes on 14.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "ICMP4Echo.h"

//Private method!
uint8_t ICMP4Echo::do_copyTo(Kernel::NetworkByteBlock *output) {
    uint8_t errors = 0;
    errors += output->appendOneByte(header.type);
    errors += output->appendOneByte(header.code);
    errors += output->appendTwoBytesSwapped(header.checksum);
    errors += output->appendTwoBytesSwapped(echoMessage.identifier);
    errors += output->appendTwoBytesSwapped(echoMessage.sequenceNumber);

    return errors;
}

//Private method!
uint8_t ICMP4Echo::do_parse(Kernel::NetworkByteBlock *input) {
    if (input->bytesRemaining() < (sizeof header + sizeof echoMessage)) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->readOneByteTo(&header.type);
    errors += input->readOneByteTo(&header.code);
    errors += input->readTwoBytesSwappedTo(&header.checksum);
    errors += input->readTwoBytesSwappedTo(&echoMessage.identifier);
    errors += input->readTwoBytesSwappedTo(&echoMessage.sequenceNumber);

    return errors;
}

//Private method!
ICMP4Message::ICMP4MessageType ICMP4Echo::do_getICMP4MessageType() {
    return ICMP4MessageType::ECHO;
}

uint16_t ICMP4Echo::do_getLengthInBytes() {
    return sizeof(header) + sizeof echoMessage;
}

ICMP4Echo::ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber) {
    header.type = 8; //8 for echo, 0 for echo reply (RFC792)
    header.code = 0;
    header.checksum = 0;
    echoMessage.identifier = identifier;
    echoMessage.sequenceNumber = sequenceNumber;
}

ICMP4EchoReply *ICMP4Echo::buildEchoReply() const {
    return new ICMP4EchoReply(
            this->echoMessage.identifier,
            this->echoMessage.sequenceNumber
    );
}

uint16_t ICMP4Echo::getSequenceNumber() const {
    return echoMessage.sequenceNumber;
}
