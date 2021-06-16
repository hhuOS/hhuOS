//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDP4DATAGRAM_H
#define HHUOS_UDP4DATAGRAM_H

//TODO: Verify values with RFC Sources
#define UDP4DATAPART_MAX_LENGTH 1500
#define UDP4HEADER_MAX_LENGTH 0xffff

#include <kernel/network/internet/IP4DataPart.h>
#include <kernel/network/udp/sockets/UDP4Port.h>
#include "UDP4Header.h"

class UDP4Datagram final : public IP4DataPart {
private:
    UDP4Header *header = nullptr;
    NetworkByteBlock *dataBytes = nullptr;

public:
    UDP4Datagram(UDP4Port *sourcePort, UDP4Port *destinationPort, void *outgoingBytes, size_t dataLength);

    ~UDP4Datagram();

    uint8_t copyTo(NetworkByteBlock *output) override;

    size_t getLengthInBytes() override;

    IP4ProtocolType getIP4ProtocolType() override;
};


#endif //HHUOS_UDP4DATAGRAM_H
