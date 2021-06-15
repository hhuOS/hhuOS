//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H

#define ETHERNETDATAPART_MAX_LENGTH 1500
#define ETHERNETHEADER_MAX_LENGTH 14

#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/internet/IP4Datagram.h>
#include <kernel/network/arp/ARPMessage.h>
#include "EthernetAddress.h"
#include "EthernetDataPart.h"
#include "EthernetHeader.h"

class EthernetFrame final {
private:
    EthernetHeader *header = nullptr;
    EthernetDataPart *ethernetDataPart = nullptr;

public:
    EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart);

    EthernetFrame() = default;

    virtual ~EthernetFrame();

    [[nodiscard]] EthernetDataPart::EtherType getEtherType() const;

    uint8_t copyTo(NetworkByteBlock *output);

    uint16_t getLengthInBytes();

    void setSourceAddress(EthernetAddress *source);
};


#endif //HHUOS_ETHERNETFRAME_H
