//
// Created by hannes on 17.05.21.
//

#include "UDPDatagram.h"

uint8_t UDPDatagram::copyTo(NetworkByteBlock *byteBlock) {
    return 1;
}

size_t UDPDatagram::getLengthInBytes() {
    return 1;
}

IP4DataPart::IP4ProtocolType UDPDatagram::getIP4ProtocolType() {
    return IP4ProtocolType::INVALID;
}

uint8_t UDPDatagram::parse(NetworkByteBlock *input) {
    return 1;
}