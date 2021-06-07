//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDPDATAGRAM_H
#define HHUOS_UDPDATAGRAM_H


#include <kernel/network/internet/IP4DataPart.h>

class UDPDatagram final : public IP4DataPart {

public:
    UDPDatagram() = default;

    ~UDPDatagram() = default;

    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    IP4ProtocolType getIP4ProtocolType() override;

    uint8_t parseHeader(NetworkByteBlock *input) override;
};


#endif //HHUOS_UDPDATAGRAM_H
