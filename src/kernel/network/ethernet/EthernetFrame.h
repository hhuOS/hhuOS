//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H


#include <kernel/network/NetworkDataPart.h>
#include "EthernetAddress.h"

class EthernetFrame {
private:
    EthernetAddress *destinationAddress;
    EthernetAddress *sourceAddress;
    uint16_t protocolType;
    NetworkDataPart *dataPart;
public:
    EthernetFrame(EthernetAddress *destinationAddress, uint16_t protocolType, NetworkDataPart *dataPart);

    EthernetFrame(void *packet, uint16_t length);

    void *getDataAsByteBlock();

    uint16_t getLength();
};


#endif //HHUOS_ETHERNETFRAME_H
