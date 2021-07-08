//
// Created by hannes on 27.06.21.
//

#ifndef HHUOS_UDP4INPUTENTRY_H
#define HHUOS_UDP4INPUTENTRY_H


#include <kernel/network/udp/UDP4Header.h>
#include <kernel/network/internet/IP4Header.h>

class UDP4InputEntry {
private:
    UDP4Header *udp4Header = nullptr;
    IP4Header *ip4Header = nullptr;
    Kernel::NetworkByteBlock *input = nullptr;

public:
    UDP4InputEntry(UDP4Header *udp4Header, IP4Header *ip4Header, Kernel::NetworkByteBlock *input);

    virtual ~UDP4InputEntry();

    uint8_t copyTo(uint16_t *totalBytesRead, void *targetBuffer, uint16_t length, IP4Header **ip4HeaderVariable,
                   UDP4Header **udp4HeaderVariable);
};


#endif //HHUOS_UDP4INPUTENTRY_H
