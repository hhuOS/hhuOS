//
// Created by hannes on 17.05.21.
//

#include "UDP4Datagram.h"

uint8_t UDP4Datagram::copyTo(NetworkByteBlock *byteBlock) {
    return 1;
}

size_t UDP4Datagram::getLengthInBytes() {
    return 1;
}

IP4DataPart::IP4ProtocolType UDP4Datagram::getIP4ProtocolType() {
    return IP4ProtocolType::INVALID;
}

uint8_t UDP4Datagram::parseHeader(NetworkByteBlock *input) {
    return 1;
}