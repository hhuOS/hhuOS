//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4DATAGRAM_H
#define HHUOS_IP4DATAGRAM_H

#include <cinttypes>
#include "IP4Address.h"
#include "kernel/network/NetworkDataPart.h"

class IP4Datagram : public NetworkDataPart {
private:
    uint8_t protocol;
    IP4Address *sourceAddress;
    IP4Address *destinationAddress;
    NetworkDataPart *dataPart;
public:
    IP4Datagram(uint8_t protocol, IP4Address *destinationAddress, NetworkDataPart *dataPart);

    IP4Datagram(NetworkDataPart *dataPart);

    uint8_t getProtocol() const;

    IP4Address *getSourceAddress() const;

    IP4Address *getDestinationAddress() const;

    NetworkDataPart *getDataPart() const;

    void setSourceAddress(IP4Address *sourceAddress);

    void setDestinationAddress(IP4Address *destinationAddress);

    void setDataPart(NetworkDataPart *dataPart);

    void *getData() override;

    uint16_t getLength() override;

    uint8_t getAProtocol() const;
};


#endif //HHUOS_IP4DATAGRAM_H
