//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4DESTINATIONUNREACHABLE_H
#define HHUOS_ICMP4DESTINATIONUNREACHABLE_H


#include <kernel/network/internet/icmp/ICMP4Message.h>

class ICMP4DestinationUnreachable : public ICMP4Message {
public:
    ICMP4DestinationUnreachable();

    ICMP4DestinationUnreachable(IP4DataPart *dataPart);

    void *getMemoryAddress() override;

    uint16_t getLength() override;
};


#endif //HHUOS_ICMP4DESTINATIONUNREACHABLE_H
