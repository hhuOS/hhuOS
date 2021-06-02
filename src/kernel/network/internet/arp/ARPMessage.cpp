//
// Created by hannes on 02.06.21.
//

#include "ARPMessage.h"

ARPMessage::ARPMessage(NetworkByteBlock *input) : input(input) {}

uint8_t ARPMessage::copyDataTo(NetworkByteBlock *byteBlock) {
    return 0;
}

size_t ARPMessage::getLengthInBytes() {
    return 0;
}

EthernetDataPart::EtherType ARPMessage::getEtherType() {
    return EtherType::INVALID;
}

uint8_t ARPMessage::parseInput() {
    return 1;
}
