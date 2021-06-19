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

    [[nodiscard]] EthernetDataPart::EtherType getEtherType() const;

    static size_t getHeaderLength();

    void setSourceAddress(EthernetAddress *address);

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

    uint8_t parse(Kernel::NetworkByteBlock *input);

    virtual ~EthernetHeader();
};


#endif //HHUOS_ETHERNETHEADER_H
