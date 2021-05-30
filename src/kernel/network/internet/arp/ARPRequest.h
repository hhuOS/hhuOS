//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ARPREQUEST_H
#define HHUOS_ARPREQUEST_H

#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/network/internet/addressing/IP4Address.h>

class ARPRequest : public EthernetDataPart {
private:
    IP4Address *ip4Address;

public:
    ARPRequest(IP4Address *ip4Address);

    uint8_t copyDataTo(uint8_t *byteBlock) override;

    uint16_t getLengthInBytes() override;

    EtherType getEtherType() override;
};


#endif //HHUOS_ARPREQUEST_H
