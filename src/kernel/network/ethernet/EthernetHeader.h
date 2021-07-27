//
// Created by hannes on 15.06.21.
//

#ifndef HHUOS_ETHERNETHEADER_H
#define HHUOS_ETHERNETHEADER_H


#include "EthernetAddress.h"
#include "EthernetDataPart.h"

class EthernetHeader {
private:
    EthernetAddress *sourceAddress = nullptr, *destinationAddress = nullptr;
    EthernetDataPart::EtherType etherType = EthernetDataPart::EtherType::INVALID;
public:
    EthernetHeader(EthernetAddress *destinationAddress, EthernetDataPart *dataPart);

    EthernetHeader() = default;

    virtual ~EthernetHeader();

    [[nodiscard]] EthernetDataPart::EtherType getEtherType() const;

    static uint16_t length();

    static uint16_t maximumFrameLength();

    bool destinationIs(EthernetAddress *otherAddress);

    uint8_t setSourceAddress(EthernetAddress *address);

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

    uint8_t parse(Kernel::NetworkByteBlock *input);

    EthernetAddress *getDestinationAddress();
};


#endif //HHUOS_ETHERNETHEADER_H
