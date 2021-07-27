//
// Created by hannes on 08.07.21.
//

#include "IP4DataPart.h"

IP4DataPart::IP4DataPart() = default;

IP4DataPart::~IP4DataPart() = default;

IP4DataPart::IP4ProtocolType IP4DataPart::getIP4ProtocolType() {
    return do_getIP4ProtocolType();
}

IP4DataPart::IP4ProtocolType IP4DataPart::parseIntAsIP4ProtocolType(uint8_t value) {
    switch (value) {
        case 1:
            return IP4ProtocolType::ICMP4;
        case 17:
            return IP4ProtocolType::UDP;
        default:
            return IP4ProtocolType::INVALID;
    }
}

uint8_t IP4DataPart::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }
    uint16_t lengthInBytes = this->length();
    if (lengthInBytes == 0 || output->bytesRemaining() < lengthInBytes) {
        return 1;
    }
    return do_copyTo(output);
}

uint16_t IP4DataPart::length() {
    return do_length();
}