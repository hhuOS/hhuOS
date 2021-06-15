//
// Created by hannes on 15.06.21.
//

#ifndef HHUOS_IP4HEADER_H
#define HHUOS_IP4HEADER_H


#include <cstdint>
#include <kernel/network/internet/addressing/IP4Address.h>
#include "IP4DataPart.h"

class IP4Header {
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

        //solid default value, can be set from constructor if necessary
        uint8_t timeToLive = 64;

        uint8_t protocolType = 0;
        uint16_t headerChecksum = 0;
        uint8_t sourceAddress[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
        uint8_t destinationAddress[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
    } ip4Header_t;

    ip4Header_t header;
    IP4Address *sourceAddress = nullptr, *destinationAddress = nullptr;

public:
    IP4Header(IP4Address *destinationAddress, IP4DataPart *dataPart);

    IP4Header() = default;

    IP4DataPart::IP4ProtocolType getIP4ProtocolType() const;

    IP4Address *getDestinationAddress();

    IP4Address *getSourceAddress();

    void setSourceAddress(IP4Address *address);

    virtual ~IP4Header();

    size_t getTotalLength() const;

    size_t getSize() const;

    uint8_t copyTo(NetworkByteBlock *output);

    uint8_t parse(NetworkByteBlock *input);
};


#endif //HHUOS_IP4HEADER_H
