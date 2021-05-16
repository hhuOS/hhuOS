//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H


#include "EthernetAddress.h"
#include "EtherType.h"
#include "EthernetDataPart.h"

class EthernetFrame {
private:
    EthernetAddress *destinationAddress;
    EthernetAddress *sourceAddress;
    EtherType etherType;
    EthernetDataPart *dataPart;
public:
    EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *dataPart);

    EthernetFrame(void *packet, uint16_t length);

    EthernetAddress *getDestinationAddress() const;

    EthernetAddress *getSourceAddress() const;

    EtherType getEtherType() const;

    EthernetDataPart *getDataPart() const;

    void *getDataAsByteBlock();

    uint16_t getLength();
};


#endif //HHUOS_ETHERNETFRAME_H
