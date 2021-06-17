//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDP4DATAGRAM_H
#define HHUOS_UDP4DATAGRAM_H

#include <kernel/network/internet/IP4DataPart.h>
#include "UDP4Header.h"

class UDP4Datagram final : public IP4DataPart {
private:
    UDP4Header *header = nullptr;
    Kernel::NetworkByteBlock *dataBytes = nullptr;

public:
    UDP4Datagram(uint16_t sourcePort, uint16_t destinationPort, Kernel::NetworkByteBlock *dataBytes);

    ~UDP4Datagram();

    uint8_t copyTo(Kernel::NetworkByteBlock *output) override;

    size_t getLengthInBytes() override;

    IP4ProtocolType getIP4ProtocolType() override;
};


#endif //HHUOS_UDP4DATAGRAM_H
