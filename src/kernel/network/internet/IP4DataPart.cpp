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
