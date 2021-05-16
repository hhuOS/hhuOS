//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ARPREQUEST_H
#define HHUOS_ARPREQUEST_H


#include <kernel/network/internet/IP4Address.h>
#include <kernel/network/NetworkDataPart.h>

class ARPRequest : public NetworkDataPart{
private:
    IP4Address *ip4Address;

public:
    ARPRequest(IP4Address *ip4Address);

    void *getData() override;

    uint16_t getLength() override;
};


#endif //HHUOS_ARPREQUEST_H
