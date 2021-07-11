//
// Created by hannes on 08.07.21.
//

#include "ICMP4Message.h"

ICMP4Message::ICMP4Message() = default;

ICMP4Message::~ICMP4Message() = default;

uint8_t ICMP4Message::fillChecksumField() {
    if (header.checksum != 0) {
        //Header checksum already set!
        return 1;
    }

    auto *headerAsBytes = new Kernel::NetworkByteBlock(length());
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
    auto *headerAsBytes = new Kernel::NetworkByteBlock(length());
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

IP4DataPart::IP4ProtocolType ICMP4Message::do_getIP4ProtocolType() {
    return IP4DataPart::ICMP4;
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

ICMP4Message::ICMP4MessageType ICMP4Message::parseByteAsICMP4MessageType(uint8_t type) {
    switch (type) {
        case 0:
            return ICMP4MessageType::ECHO_REPLY;
        case 3:
            return ICMP4MessageType::DESTINATION_UNREACHABLE;
        case 4:
            return ICMP4MessageType::SOURCE_QUENCH;
        case 5:
            return ICMP4MessageType::REDIRECT;
        case 8:
            return ICMP4MessageType::ECHO;
        case 11:
            return ICMP4MessageType::TIME_EXCEEDED;
        case 12:
            return ICMP4MessageType::PARAMETER_PROBLEM;
        case 13:
            return ICMP4MessageType::TIMESTAMP;
        case 14:
            return ICMP4MessageType::TIMESTAMP_REPLY;
        case 15:
            return ICMP4MessageType::INFORMATION_REQUEST;
        case 16:
            return ICMP4MessageType::INFORMATION_REPLY;
        default:
            return ICMP4MessageType::INVALID;
    }
}


