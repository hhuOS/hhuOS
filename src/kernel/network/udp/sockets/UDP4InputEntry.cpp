//
// Created by hannes on 27.06.21.
//

#include "UDP4InputEntry.h"

UDP4InputEntry::UDP4InputEntry(UDP4Header *udp4Header, IP4Header *ip4Header, Kernel::NetworkByteBlock *input)
        : udp4Header(udp4Header), ip4Header(ip4Header), input(input) {}

uint8_t UDP4InputEntry::copyTo(size_t *totalBytesRead, void *targetBuffer, size_t length, IP4Header **ip4HeaderVariable,
                               UDP4Header **udp4HeaderVariable) {
    if (totalBytesRead != nullptr) {
        //count bytes read if requested
        *totalBytesRead = input->bytesRemaining();
    }
    if (length > input->bytesRemaining()) {
        length = input->bytesRemaining();
    }

    //Cleanup if reading fails
    if (input->readStraightTo(targetBuffer, length)) {
        return 1;
    }
    if (totalBytesRead != nullptr) {
        *totalBytesRead = *totalBytesRead - input->bytesRemaining();
    }
    if (ip4HeaderVariable == nullptr) {
        //delete IP4Header if not requested
        delete this->ip4Header;
    } else {
        *ip4HeaderVariable = this->ip4Header;
    }
    if (udp4HeaderVariable == nullptr) {
        //delete UDP4Header if not requested
        delete this->udp4Header;
    } else {
        *udp4HeaderVariable = this->udp4Header;
    }

    //Processing finally done, cleanup input
    delete input;

    return 0;
}
