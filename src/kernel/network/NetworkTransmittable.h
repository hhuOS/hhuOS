//
// Created by hannes on 08.07.21.
//

#ifndef HHUOS_NETWORKTRANSMITTABLE_H
#define HHUOS_NETWORKTRANSMITTABLE_H


#include "NetworkByteBlock.h"

class NetworkTransmittable {
public:
    virtual ~NetworkTransmittable();

    // forbid copying
    NetworkTransmittable(NetworkTransmittable const &) = delete;

    NetworkTransmittable &operator=(NetworkTransmittable const &) = delete;

    size_t getLengthInBytes();

    uint8_t copyTo(Kernel::NetworkByteBlock *output);

protected:
    NetworkTransmittable();

private:
    virtual uint8_t do_copyTo(Kernel::NetworkByteBlock *output) = 0;

    virtual size_t do_getLengthInBytes() = 0;
};


#endif //HHUOS_NETWORKTRANSMITTABLE_H
