//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H

#include "EthernetHeader.h"

class EthernetFrame final {
private:
    EthernetHeader *ethernetHeader = nullptr;
    EthernetDataPart *ethernetDataPart = nullptr;

public:
    EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart);

    EthernetFrame() = default;

    virtual ~EthernetFrame();

    uint16_t length();

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

    uint8_t setSourceAddress(EthernetAddress *source);
};


#endif //HHUOS_ETHERNETFRAME_H
