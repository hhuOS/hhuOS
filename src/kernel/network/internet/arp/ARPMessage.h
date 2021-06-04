//
// Created by hannes on 02.06.21.
//

#ifndef HHUOS_ARPMESSAGE_H
#define HHUOS_ARPMESSAGE_H


#include <kernel/network/NetworkByteBlock.h>
#include <kernel/network/ethernet/EthernetDataPart.h>

class ARPMessage : public EthernetDataPart {
public:
    ARPMessage() = default;
    //TODO: Let other ARPMessages extend this one!

    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    EtherType getEtherType() override;

    uint8_t parse(NetworkByteBlock *input) override;

    void freeMemory() override;
};


#endif //HHUOS_ARPMESSAGE_H
