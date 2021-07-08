//
// Created by hannes on 08.07.21.
//

#include "ICMP4Message.h"

ICMP4Message::ICMP4Message() = default;

ICMP4Message::~ICMP4Message() = default;

IP4DataPart::IP4ProtocolType ICMP4Message::getIP4ProtocolType() {
    return IP4ProtocolType::ICMP4;
}

uint8_t ICMP4Message::fillChecksumField() {
    if (header.checksum != 0) {
        //Header checksum already set!
        return 1;
    }

    auto *headerAsBytes = new Kernel::NetworkByteBlock(getLengthInBytes());
    if (this->copyTo(headerAsBytes)) {
        delete headerAsBytes;
        return 1;
    }
    headerAsBytes->resetIndex();

    uint16_t calculationResult = 0;
    if (IP4Header::calculateInternetChecksum(&calculationResult, headerAsBytes)) {
        delete headerAsBytes;
        return 1;
    }
    header.checksum = calculationResult;
    delete headerAsBytes;
    return 0;
}

bool ICMP4Message::checksumIsValid() {
    if (header.checksum == 0) {
        //Header checksum not parsed!
        return false;
    }

    uint16_t calculationResult = 0;
    auto *headerAsBytes = new Kernel::NetworkByteBlock(getLengthInBytes());
    if (this->copyTo(headerAsBytes)) {
        delete headerAsBytes;
        return false;
    }
    headerAsBytes->resetIndex();

    if (IP4Header::calculateInternetChecksum(&calculationResult, headerAsBytes)) {
        delete headerAsBytes;
        return false;
    }

    delete headerAsBytes;
    return calculationResult == 0;
}

uint8_t ICMP4Message::parse(Kernel::NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() <= sizeof header) {
        return 1;
    }
    return do_parse(input);
}

ICMP4Message::ICMP4MessageType ICMP4Message::getICMP4MessageType() {
    return do_getICMP4MessageType();
}


