//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H

#define ETHERNETDATAPART_MAX_LENGTH 1500
#define ETHERNETHEADER_MAX_LENGTH 14

#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/internet/IP4Datagram.h>
#include <kernel/network/internet/arp/ARPMessage.h>
#include "EthernetAddress.h"
#include "EthernetDataPart.h"

class EthernetFrame {
private:
    //Defined internally, should not be visible outside
    //Usage of IP4Datagram should only happen via given public methods
    //-> changing our header's internal representation is possible at any time then!
    typedef struct ethernetHeader {
        uint8_t destinationAddress[MAC_SIZE]{0, 0, 0, 0, 0, 0};
        uint8_t sourceAddress[MAC_SIZE]{0, 0, 0, 0, 0, 0};
        uint16_t etherType = 0;
    } ethHeader_t;

    ethHeader_t header;

    EthernetDataPart *ethernetDataPart = nullptr;

public:
    EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart);

    EthernetFrame() = default;

    ~EthernetFrame() = default;

    [[nodiscard]] EthernetDataPart *getEthernetDataPart() const;

    [[nodiscard]] EthernetDataPart::EtherType getEtherType() const;

    uint8_t copyTo(NetworkByteBlock *output);

    uint16_t getLengthInBytes();

    void setSourceAddress(EthernetAddress *source);

    uint8_t parse(NetworkByteBlock *input);
};


#endif //HHUOS_ETHERNETFRAME_H
