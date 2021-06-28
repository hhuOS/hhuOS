//
// Created by hannes on 27.06.21.
//

#include "UDP4InputEntry.h"

UDP4InputEntry::UDP4InputEntry(UDP4Header *udp4Header, IP4Header *ip4Header, Kernel::NetworkByteBlock *input)
        : udp4Header(udp4Header), ip4Header(ip4Header), input(input) {}

UDP4InputEntry::~UDP4InputEntry() {
    //'delete' simply ignores null objects!
    //-> no check for ==nullptr necessary here
    delete udp4Header;
    delete ip4Header;
    delete input;
}

uint8_t UDP4InputEntry::copyTo(size_t *totalBytesRead, void *targetBuffer, size_t length, IP4Header **ip4HeaderVariable,
                               UDP4Header **udp4HeaderVariable) {
    if (udp4Header== nullptr || ip4Header == nullptr || input == nullptr) {
        return 1;
    }

    if (totalBytesRead != nullptr) {
        //count bytes read if requested
        *totalBytesRead = input->bytesRemaining();
    }
    if (length > input->bytesRemaining()) {
        length = input->bytesRemaining();
    }

    //Cleanup if copy to target buffer fails
    if (input->readStraightTo(targetBuffer, length)) {
        return 1;
    }
    if (totalBytesRead != nullptr) {
        //set value for bytes read if requested
        *totalBytesRead = *totalBytesRead - input->bytesRemaining();
    }
    if (udp4HeaderVariable != nullptr) {
        *udp4HeaderVariable = this->udp4Header;
        //udp4Header is in use in our application here
        //-> set it to null locally so that 'delete this->udp4Header' has no effect
        this->udp4Header = nullptr;
    }
    if (ip4HeaderVariable != nullptr) {
        *ip4HeaderVariable = this->ip4Header;
        //ip4header is in use in our application here
        //-> set it to null locally so that 'delete this->ip4Header' has no effect
        this->ip4Header = nullptr;
    }
    return 0;
}
