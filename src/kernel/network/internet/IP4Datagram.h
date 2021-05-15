//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAGRAM_H
#define HHUOS_IP4DATAGRAM_H

#include <cinttypes>
#include "IP4Address.h"
#include "kernel/network/NetworkDataPart.h"

class IP4Datagram {
private:
    IP4Address *sourceAddress;
    IP4Address *destinationAddress;
    NetworkDataPart *dataPart;
public:
    IP4Datagram(IP4Address *destinationAddress, NetworkDataPart *dataPart);

    IP4Address *getSourceAddress() const;

    IP4Address *getDestinationAddress() const;

    NetworkDataPart *getDataPart() const;

    void setSourceAddress(IP4Address *sourceAddress);

    void setDestinationAddress(IP4Address *destinationAddress);

    void setDataPart(NetworkDataPart *dataPart);
};


#endif //HHUOS_IP4DATAGRAM_H
