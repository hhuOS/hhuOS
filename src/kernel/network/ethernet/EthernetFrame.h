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
        uint8_t destinationAddress[6]{0, 0, 0, 0, 0, 0};
        uint8_t sourceAddress[6]{0, 0, 0, 0, 0, 0};
        uint16_t etherType = 0;
    } ethHeader_t;

    ethHeader_t header;
    size_t headerLengthInBytes = sizeof(header);
    EthernetDataPart *ethernetDataPart = nullptr;
    NetworkByteBlock *input = nullptr;

public:
    EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart);

    explicit EthernetFrame(NetworkByteBlock *input);

    virtual ~EthernetFrame();

    [[nodiscard]] EthernetDataPart::EtherType getEtherType() const;

    uint8_t copyDataTo(NetworkByteBlock *byteBlock);

    uint16_t getTotalLengthInBytes();

    void setSourceAddress(EthernetAddress *sourceAddress);

    uint8_t parseInput();

    IP4Datagram *buildIP4DatagramWithInput();

    ARPMessage *buildARPMessageWithInput();
};


#endif //HHUOS_ETHERNETFRAME_H
