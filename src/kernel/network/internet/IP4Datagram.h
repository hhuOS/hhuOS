//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAGRAM_H
#define HHUOS_IP4DATAGRAM_H

#include <cinttypes>
#include <kernel/network/ethernet/EthernetDataPart.h>
#include <kernel/network/ethernet/EtherType.h>
#include "IP4Address.h"
#include "IP4DataPart.h"
#include "IP4ProtocolType.h"

class IP4Datagram : public EthernetDataPart {
private:
    IP4ProtocolType ip4ProtocolType;
    IP4Address *sourceAddress;
    IP4Address *destinationAddress;
    IP4DataPart *ip4DataPart;
public:
    IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart);

    IP4Datagram(EthernetDataPart *ethernetDataPart);

    IP4ProtocolType getIp4ProtocolType() const;

    IP4Address *getSourceAddress() const;

    void setSourceAddress(IP4Address *sourceAddress);

    IP4Address *getDestinationAddress() const;

    IP4DataPart *getIp4DataPart() const;

    void *getMemoryAddress() override;

    uint16_t getLength() override;

    EtherType getEtherType();
};


#endif //HHUOS_IP4DATAGRAM_H
