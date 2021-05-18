//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMP4ECHO_H
#define HHUOS_ICMP4ECHO_H

#include <kernel/network/internet/icmp/ICMP4Message.h>

class ICMP4Echo : public ICMP4Message {
public:
    //Sending constructor
    ICMP4Echo();

    //Receiveing constructor
    ICMP4Echo(IP4DataPart *dataPart);

    void *getMemoryAddress() override;

    uint16_t getLength() override;
};


#endif //HHUOS_ICMP4ECHO_H
