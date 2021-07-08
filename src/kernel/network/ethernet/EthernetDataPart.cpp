//
// Created by hannes on 08.07.21.
//

#include "EthernetDataPart.h"

uint8_t EthernetDataPart::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr || output->bytesRemaining() < this->getLengthInBytes()) {
        return 1;
    }
    return do_copyTo(output);
}

EthernetDataPart::~EthernetDataPart() = default;
