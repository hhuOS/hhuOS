//
// Created by hannes on 18.05.21.
//

#ifndef HHUOS_ICMP4TIMEEXCEEDED_H
#define HHUOS_ICMP4TIMEEXCEEDED_H


#include <kernel/network/internet/icmp/ICMP4Message.h>

class ICMP4TimeExceeded : public ICMP4Message {
    //Sending constructor
    ICMP4TimeExceeded();

    //Receiveing constructor
    ICMP4TimeExceeded(IP4DataPart *dataPart);

    void *getMemoryAddress() override;

    uint16_t getLengthInBytes() override;
};


#endif //HHUOS_ICMP4TIMEEXCEEDED_H
