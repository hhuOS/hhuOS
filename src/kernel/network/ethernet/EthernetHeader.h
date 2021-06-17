//
// Created by hannes on 15.06.21.
//

#ifndef HHUOS_ETHERNETHEADER_H
#define HHUOS_ETHERNETHEADER_H


#include <cstdint>
#include "EthernetAddress.h"
#include "EthernetDataPart.h"

class EthernetHeader {
private:
    //Defined internally, should not be visible outside
    //Usage of EthernetFrame should only happen via given public methods
    //-> changing our header's internal representation is possible at any time then!
    typedef struct ethernetHeader {
        uint8_t destinationAddress[MAC_SIZE]{0, 0, 0, 0, 0, 0};
        uint8_t sourceAddress[MAC_SIZE]{0, 0, 0, 0, 0, 0};
        uint16_t etherType = 0;
    } ethHeader_t;

    ethHeader_t header;

    EthernetAddress *sourceAddress = nullptr, *destinationAddress = nullptr;
public:
    EthernetHeader(EthernetAddress *destinationAddress, EthernetDataPart *dataPart);

    EthernetHeader() = default;

    [[nodiscard]] EthernetDataPart::EtherType getEtherType() const;

    size_t getSize();

    void setSourceAddress(EthernetAddress *address);

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

    uint8_t parse(Kernel::NetworkByteBlock *input);
};


#endif //HHUOS_ETHERNETHEADER_H
