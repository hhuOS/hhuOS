//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAGRAM_H
#define HHUOS_IP4DATAGRAM_H

#include <kernel/network/ethernet/EthernetDataPart.h>
#include "IP4Header.h"

class IP4Datagram final : public EthernetDataPart {
private:
    IP4Header *header = nullptr;
    IP4DataPart *ip4DataPart = nullptr;
public:

    IP4Datagram(IP4Address *destinationAddress, IP4DataPart *ip4DataPart);

    IP4Datagram() = default;

    virtual ~IP4Datagram();

    [[nodiscard]] IP4Address *getDestinationAddress() const;

    EtherType getEtherType() override;

    size_t getLengthInBytes() override;

    uint8_t setSourceAddress(IP4Address *source);

    uint8_t copyTo(Kernel::NetworkByteBlock *output) override;

    uint8_t fillHeaderChecksum();

    String asString(String spacing) override;
};


#endif //HHUOS_IP4DATAGRAM_H
