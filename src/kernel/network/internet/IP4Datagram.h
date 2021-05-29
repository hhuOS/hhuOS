//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAGRAM_H
#define HHUOS_IP4DATAGRAM_H

#include <cinttypes>
#include <kernel/network/ethernet/EthernetDataPart.h>
#include "addressing/IP4Address.h"
#include "IP4DataPart.h"
#include "IP4HeaderChecksum.h"

class IP4Datagram : public EthernetDataPart {
private:
    //Defined internally, should not be visible outside
    //Usage of IP4Datagram should only happen via given public methods
    //-> changing our header's internal representation is possible at any time then!
    typedef struct ip4header {
        //First four bits are value "4" -> IPv4
        //Second four bits are value "5" -> 5 "lines" header length, 4 Bytes per line
        uint8_t version_headerLength = 0x45;

        //standard type of service, no priority etc.
        uint8_t typeOfService = 0;

        uint16_t totalLength = 0;

        //fragmentation not used here, fragment parameters not set
        uint16_t identification = 0;
        uint16_t flags_fragmentOffset = 0;

        //solid default value for small LANs, can be set from constructor if necessary
        uint8_t timeToLive = 64;

        uint8_t protocolType = 0;
        uint16_t headerChecksum = 0;
        uint32_t sourceAddress = 0;
        uint32_t destinationAddress = 0;
    } ip4Header_t;

    ip4Header_t header;
    IP4DataPart *ip4DataPart;
public:

    IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart);

    IP4Datagram(EthernetDataPart *ethernetDataPart);

    void setSourceAddress(IP4Address *sourceAddress);

    IP4DataPart::IP4ProtocolType getIP4ProtocolType() const;

    IP4Address *getSourceAddress() const;

    IP4Address *getDestinationAddress() const;

    IP4DataPart *getIp4DataPart() const;

    void *getMemoryAddress() override;

    uint16_t getLengthInBytes() override;

    EtherType getEtherType() override;
};


#endif //HHUOS_IP4DATAGRAM_H
