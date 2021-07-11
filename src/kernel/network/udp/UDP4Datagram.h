//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_UDP4DATAGRAM_H
#define HHUOS_UDP4DATAGRAM_H

#include <kernel/network/internet/IP4DataPart.h>
#include "UDP4Header.h"

class UDP4Datagram final : public IP4DataPart {
private:
    UDP4Header *udp4Header = nullptr;
    Kernel::NetworkByteBlock *udp4DataBytes = nullptr;

    uint8_t do_copyTo(Kernel::NetworkByteBlock *output) final;

    IP4ProtocolType do_getIP4ProtocolType() override;

    uint16_t do_getLengthInBytes() override;

public:

    UDP4Datagram(uint16_t sourcePort, uint16_t destinationPort, Kernel::NetworkByteBlock *dataBytes);

    ~UDP4Datagram() override;
};


#endif //HHUOS_UDP4DATAGRAM_H
