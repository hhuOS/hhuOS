//
// Created by hannes on 10.06.21.
//

#ifndef HHUOS_TRANSMITTABLESTRING_H
#define HHUOS_TRANSMITTABLESTRING_H


#include <cstdio>
#include <kernel/network/NetworkByteBlock.h>

class TransmittableString {
private:
    NetworkByteBlock *characters;
public:
    explicit TransmittableString(size_t length);

    virtual ~TransmittableString();

    void append(char *chars, size_t length);
};


#endif //HHUOS_TRANSMITTABLESTRING_H
