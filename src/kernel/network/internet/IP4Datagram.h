//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAGRAM_H
#define HHUOS_IP4DATAGRAM_H

#include "lib/libc/stdlib.h"
#include <kernel/network/ethernet/EthernetDataPart.h>
#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/udp/UDP4Datagram.h>
#include "addressing/IP4Address.h"
#include "IP4DataPart.h"
#include "IP4Header.h"

class IP4Datagram final : public EthernetDataPart {
private:
    IP4Header *header = nullptr;
    IP4DataPart *ip4DataPart = nullptr;
public:

    IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart);

    IP4Datagram() = default;

    virtual ~IP4Datagram();

    EtherType getEtherType() override;

    uint8_t copyTo(Kernel::NetworkByteBlock *output) override;

    size_t getLengthInBytes() override;

    void setSourceAddress(IP4Address *source);

    [[nodiscard]] IP4Address *getDestinationAddress() const;

    [[nodiscard]] IP4DataPart::IP4ProtocolType getIP4ProtocolType() const;
};


#endif //HHUOS_IP4DATAGRAM_H
