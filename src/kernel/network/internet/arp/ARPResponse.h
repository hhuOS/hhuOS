//
// Created by hannes on 16.05.21.
//

#ifndef HHUOS_ARPRESPONSE_H
#define HHUOS_ARPRESPONSE_H


#include <kernel/network/internet/IP4Address.h>
#include <kernel/network/ethernet/EthernetAddress.h>
#include <kernel/network/ethernet/EthernetDataPart.h>

class ARPResponse {
private:
    IP4Address *ip4Address;
    EthernetAddress *ethernetAddress;

public:
    ARPResponse(EthernetDataPart *dataPart);
};


#endif //HHUOS_ARPRESPONSE_H
