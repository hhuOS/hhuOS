//
// Created by hannes on 18.05.21.
//

#ifndef HHUOS_ICMP4TIMEEXCEEDED_H
#define HHUOS_ICMP4TIMEEXCEEDED_H


#include <kernel/network/internet/icmp/ICMP4Message.h>

class ICMP4TimeExceeded : public ICMP4Message {
public:
    //Sending constructor
    ICMP4TimeExceeded() = default;

    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    ICMP4MessageType getICMP4MessageType() override;

    uint8_t parse(NetworkByteBlock *input) override;
};


#endif //HHUOS_ICMP4TIMEEXCEEDED_H
