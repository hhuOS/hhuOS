//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ARPREQUEST_H
#define HHUOS_ARPREQUEST_H


#include <kernel/network/internet/IP4Address.h>

class ARPRequest : public EthernetDataPart{
private:
    IP4Address *ip4Address;

public:
    ARPRequest(IP4Address *ip4Address);

    void *getMemoryAddress() override;

    uint16_t getLength() override;

    EtherType getEtherType();
};


#endif //HHUOS_ARPREQUEST_H
