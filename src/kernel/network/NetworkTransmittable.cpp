//
// Created by hannes on 08.07.21.
//

#include "NetworkTransmittable.h"

NetworkTransmittable::~NetworkTransmittable() = default;

NetworkTransmittable::NetworkTransmittable() = default;

uint8_t NetworkTransmittable::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }
    size_t lengthInBytes = this->getLengthInBytes();
    if (lengthInBytes == 0 || output->bytesRemaining() < lengthInBytes) {
        return 1;
    }
    return do_copyTo(output);
}

size_t NetworkTransmittable::getLengthInBytes() {
    return do_getLengthInBytes();
}
