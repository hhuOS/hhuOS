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
    uint16_t etherType;
    NetworkDataPart *dataPart;
public:
    EthernetFrame(EthernetAddress *destinationAddress, uint16_t etherType, NetworkDataPart *dataPart);

    EthernetFrame(void *packet, uint16_t length);

    EthernetAddress *getDestinationAddress() const;

    EthernetAddress *getSourceAddress() const;

    uint16_t getProtocolType() const;

    NetworkDataPart *getDataPart() const;

    void *getDataAsByteBlock();

    uint16_t getLength();

    //Relevant Ethertypes -> full list available in RFC7042 Appendix B
    static const constexpr uint16_t ETHERTYPE_IP4 = 0x0800;
    static const constexpr uint16_t ETHERTYPE_ARP = 0x0806;
    static const constexpr uint16_t ETHERTYPE_IP6 = 0x86dd;
};


#endif //HHUOS_ETHERNETFRAME_H
