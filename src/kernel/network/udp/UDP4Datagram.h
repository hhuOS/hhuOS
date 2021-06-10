//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDPDATAGRAM_H
#define HHUOS_UDPDATAGRAM_H


#include <kernel/network/internet/IP4DataPart.h>

class UDP4Datagram final : public IP4DataPart {

public:
    UDP4Datagram() = default;

    ~UDP4Datagram() = default;

    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    IP4ProtocolType getIP4ProtocolType() override;

    uint8_t parseHeader(NetworkByteBlock *input) override;

    char *firstBytesAsChars();
};


#endif //HHUOS_UDPDATAGRAM_H
