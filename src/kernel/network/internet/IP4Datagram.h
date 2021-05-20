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
#include "IP4HeaderChecksum.h"

class IP4Datagram : public EthernetDataPart {
private:
    //Defined internally, should not be visible outside
    //Usage of IP4Datagram should only happen via given public methods
    //-> changing our header's internal representation is possible at any time then!
    typedef struct ip4header{
        uint8_t version_headerLength;
        uint8_t typeOfService;
        uint16_t totalLength;
        uint16_t identification;
        uint16_t flags_fragmentOffset;
        uint8_t timeToLive;
    } header_t;

    header_t header;
    IP4ProtocolType ip4ProtocolType;
    IP4HeaderChecksum *headerChecksum;
    IP4Address *sourceAddress;
    IP4Address *destinationAddress;
    IP4DataPart *ip4DataPart;
public:

    IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart);

    IP4Datagram(EthernetDataPart *ethernetDataPart);

    void setSourceAddress(IP4Address *sourceAddress);

    IP4ProtocolType getIp4ProtocolType() const;

    IP4Address *getSourceAddress() const;

    IP4Address *getDestinationAddress() const;

    IP4DataPart *getIp4DataPart() const;

    void *getMemoryAddress() override;

    uint16_t getLength() override;

    EtherType getEtherType() override;
};


#endif //HHUOS_IP4DATAGRAM_H
