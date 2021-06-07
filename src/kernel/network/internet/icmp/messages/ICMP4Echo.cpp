//
// Created by hannes on 14.05.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include "ICMP4Echo.h"

ICMP4Echo::ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber) {
    echoMessage.checksum = 0; //TODO: Implement checksum calculation
    echoMessage.identifier = identifier;
    echoMessage.sequenceNumber = sequenceNumber;
}

size_t ICMP4Echo::getLengthInBytes() {
    return sizeof echoMessage;
}

IP4Address *ICMP4Echo::getSourceAddress() {
    return new IP4Address(ip4Info.sourceAddress);;
}

void ICMP4Echo::setSourceAddress(IP4Address *ip4Address) {
    ip4Address->copyTo(ip4Info.sourceAddress);
}

ICMP4EchoReply *ICMP4Echo::buildEchoReply() const {
    auto *result = new ICMP4EchoReply(this->echoMessage.identifier,
                                      this->echoMessage.sequenceNumber + 1
    );
    result->setSourceAddress(new IP4Address(ip4Info.sourceAddress));
    return result;
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
    if (input == nullptr ||
        input->bytesRemaining() <= sizeof echoMessage
            ) {
        return 1;
    }
    //NOTE: The first Byte for 'type' is already read in IP4Datagram!
    //-> the next Byte in our NetworkByteBlock is 'code', the next value!
    //This is no problem here, because the 'type' value is constant '8' per definition
    uint8_t errors = 0;
    errors += input->read(&echoMessage.code);
    errors += input->read(&echoMessage.checksum);
    errors += input->read(&echoMessage.identifier);
    errors += input->read(&echoMessage.sequenceNumber);

    return errors;
}