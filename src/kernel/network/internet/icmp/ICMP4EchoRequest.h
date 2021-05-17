//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMP4ECHOREQUEST_H
#define HHUOS_ICMP4ECHOREQUEST_H

#include <cinttypes>
#include <kernel/network/internet/IP4DataPart.h>

class ICMP4EchoRequest : public IP4DataPart {
public:
    ICMP4EchoRequest();

    void *getMemoryAddress() override;

    uint16_t getLength() override;

    IP4ProtocolType getIP4ProtocolType() override;
};


#endif //HHUOS_ICMP4ECHOREQUEST_H
