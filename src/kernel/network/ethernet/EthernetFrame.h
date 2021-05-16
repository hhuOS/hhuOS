//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H


#include <kernel/network/NetworkDataPart.h>
#include "EthernetAddress.h"
#include "EtherType.h"

class EthernetFrame {
private:
    EthernetAddress *destinationAddress;
    EthernetAddress *sourceAddress;
    EtherType etherType;
    NetworkDataPart *dataPart;
public:
    EthernetFrame(EthernetAddress *destinationAddress, EtherType etherType, NetworkDataPart *dataPart);

    EthernetFrame(void *packet, uint16_t length);

    EthernetAddress *getDestinationAddress() const;

    EthernetAddress *getSourceAddress() const;

    EtherType getEtherType() const;

    NetworkDataPart *getDataPart() const;

    void *getDataAsByteBlock();

    uint16_t getLength();
};


#endif //HHUOS_ETHERNETFRAME_H
