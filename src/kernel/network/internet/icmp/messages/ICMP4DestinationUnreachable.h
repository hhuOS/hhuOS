//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4DESTINATIONUNREACHABLE_H
#define HHUOS_ICMP4DESTINATIONUNREACHABLE_H


#include <kernel/network/internet/icmp/ICMP4Message.h>
#include <kernel/network/NetworkByteBlock.h>

class ICMP4DestinationUnreachable : public ICMP4Message {
public:
    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    ICMP4MessageType getICMP4MessageType() override;

    uint8_t parse(NetworkByteBlock *input) override;

    void freeMemory() override;
};


#endif //HHUOS_ICMP4DESTINATIONUNREACHABLE_H
