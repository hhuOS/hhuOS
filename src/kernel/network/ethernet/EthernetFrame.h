//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H

#include "EthernetHeader.h"

class EthernetFrame final {
private:
    EthernetHeader *header = nullptr;
    EthernetDataPart *ethernetDataPart = nullptr;

public:
    EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart);

    EthernetFrame() = default;

    virtual ~EthernetFrame();

    size_t getLengthInBytes();

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

    uint8_t setSourceAddress(EthernetAddress *source);

    String headerAsString(const String &spacing);
};


#endif //HHUOS_ETHERNETFRAME_H
