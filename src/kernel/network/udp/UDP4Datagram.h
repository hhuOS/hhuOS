//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDP4DATAGRAM_H
#define HHUOS_UDP4DATAGRAM_H


#include <kernel/network/internet/IP4DataPart.h>
#include "UDP4DataPart.h"

class UDP4Datagram final : public IP4DataPart {
private:
    typedef struct udp4Header {
        uint16_t sourcePort = 0;
        uint16_t destinationPort = 0;
        uint16_t length = 0;
        uint16_t checksum = 0;
    } header_t;

    header_t header;

    UDP4DataPart *udp4DataPart = nullptr;

public:
    UDP4Datagram(uint16_t destinationPort, UDP4DataPart *udp4DataPart);

    UDP4Datagram() = default;

    ~UDP4Datagram() = default;

    uint8_t copyTo(NetworkByteBlock *output) override;

    size_t getLengthInBytes() override;

    IP4ProtocolType getIP4ProtocolType() override;

    uint8_t parseHeader(NetworkByteBlock *input) override;
};


#endif //HHUOS_UDP4DATAGRAM_H
