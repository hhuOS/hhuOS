//
// Created by hannes on 27.06.21.
//

#include "UDP4InputEntry.h"

UDP4InputEntry::UDP4InputEntry(UDP4Header *udp4Header, IP4Header *ip4Header, Kernel::NetworkByteBlock *content)
        : udp4Header(udp4Header), ip4Header(ip4Header), content(content) {}

uint8_t UDP4InputEntry::copyTo(size_t *totalBytesRead, void *targetBuffer, size_t length, IP4Header **ip4HeaderVariable,
                               UDP4Header **udp4HeaderVariable) {
    if (totalBytesRead != nullptr) {
        //count bytes read if requested
        *totalBytesRead = content->bytesRemaining();
    }
    if (length > content->bytesRemaining()) {
        length = content->bytesRemaining();
    }

    //Cleanup if reading fails
    if (content->readStraightTo(targetBuffer, length)) {
        return 1;
    }
    if (totalBytesRead != nullptr) {
        *totalBytesRead = *totalBytesRead - content->bytesRemaining();
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

    delete content;

    return 0;
}
