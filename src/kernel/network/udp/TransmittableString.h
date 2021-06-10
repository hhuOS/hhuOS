//
// Created by hannes on 10.06.21.
//

#ifndef HHUOS_TRANSMITTABLESTRING_H
#define HHUOS_TRANSMITTABLESTRING_H


#include <cstdio>
#include <kernel/network/NetworkByteBlock.h>
#include "UDP4DataPart.h"

class TransmittableString : public UDP4DataPart {
private:
    NetworkByteBlock *characters;
public:
    explicit TransmittableString(size_t length);

    virtual ~TransmittableString();

    void append(char *chars, size_t length);

    uint8_t copyTo(NetworkByteBlock *byteBlock) override;

    size_t getLengthInBytes() override;

    uint8_t parseData(NetworkByteBlock *input) override;
};


#endif //HHUOS_TRANSMITTABLESTRING_H
