//
// Created by hannes on 15.06.21.
//

#ifndef HHUOS_IP4HEADER_H
#define HHUOS_IP4HEADER_H


#include <kernel/network/internet/IP4Address.h>
#include "IP4DataPart.h"

class IP4Header {
private:
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

    IP4DataPart::IP4ProtocolType protocolType = IP4DataPart::IP4ProtocolType::INVALID;
    uint16_t headerChecksum = 0;

    IP4Address *sourceAddress = nullptr, *destinationAddress = nullptr;

public:

    IP4Header(IP4Address *destinationAddress, IP4DataPart *dataPart);

    IP4Header() = default;

    [[nodiscard]] static uint8_t calculateInternetChecksum(uint16_t *target, Kernel::NetworkByteBlock *content);

    [[nodiscard]] IP4DataPart::IP4ProtocolType getIP4ProtocolType() const;

    IP4Address *getDestinationAddress();

    IP4Address *getSourceAddress();

    void setSourceAddress(IP4Address *address);

    virtual ~IP4Header();

    [[nodiscard]] size_t getTotalDatagramLength() const;

    [[nodiscard]] size_t getHeaderLength() const;

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

    uint8_t parse(Kernel::NetworkByteBlock *input);

    uint8_t fillChecksumField();

    bool headerIsValid();
};


#endif //HHUOS_IP4HEADER_H
