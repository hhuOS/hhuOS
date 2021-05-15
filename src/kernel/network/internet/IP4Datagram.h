//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAGRAM_H
#define HHUOS_IP4DATAGRAM_H

#include <cinttypes>
#include "IP4Address.h"
#include "kernel/network/NetworkDataPart.h"

class IP4Datagram {
public:
    IP4Datagram(IP4Address *destinationAddress, NetworkDataPart *dataPart);
};


#endif //HHUOS_IP4DATAGRAM_H
