//
// Created by hannes on 08.07.21.
//

#include "EthernetDataPart.h"

EthernetDataPart::EthernetDataPart() = default;

EthernetDataPart::~EthernetDataPart() = default;

EthernetDataPart::EtherType EthernetDataPart::getEtherType() {
    return do_getEtherType();
}

EthernetDataPart::EtherType EthernetDataPart::parseIntAsEtherType(uint16_t value) {
    switch (value) {
        case 0x0800:
            return EthernetDataPart::EtherType::IP4;
        case 0x0806:
            return EthernetDataPart::EtherType::ARP;
        case 0x86dd:
            return EthernetDataPart::EtherType::IP6;
        default:
            return EthernetDataPart::EtherType::INVALID;
    }
}

uint8_t EthernetDataPart::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }
    uint16_t lengthInBytes = this->length();
    if (lengthInBytes == 0 || output->bytesRemaining() < lengthInBytes) {
        return 1;
    }
    return do_copyTo(output);
}

uint16_t EthernetDataPart::length() {
    return do_length();
}
