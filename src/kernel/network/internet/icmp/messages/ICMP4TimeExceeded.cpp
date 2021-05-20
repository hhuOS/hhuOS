//
// Created by hannes on 18.05.21.
//

#include "ICMP4TimeExceeded.h"

ICMP4TimeExceeded::ICMP4TimeExceeded() {}

ICMP4TimeExceeded::ICMP4TimeExceeded(IP4DataPart *dataPart) {
    //TODO: Implement parsing from input
}

void *ICMP4TimeExceeded::getMemoryAddress() {
    return nullptr;
}

uint16_t ICMP4TimeExceeded::getLengthInBytes() {
    return 0;
}