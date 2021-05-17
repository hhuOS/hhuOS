//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4ECHOREPLY_H
#define HHUOS_ICMP4ECHOREPLY_H


#include <kernel/network/internet/IP4DataPart.h>

class ICMP4EchoReply : IP4DataPart{
public:
    ICMP4EchoReply();

    ICMP4EchoReply(IP4DataPart *dataPart);

    void *getMemoryAddress() override;

    uint16_t getLength() override;

    IP4ProtocolType getIP4ProtocolType() override;
};


#endif //HHUOS_ICMP4ECHOREPLY_H
