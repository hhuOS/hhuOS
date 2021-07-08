//
// Created by hannes on 08.07.21.
//

#include "EthernetDataPart.h"

EthernetDataPart::~EthernetDataPart() = default;

uint8_t EthernetDataPart::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }
    size_t lengthInBytes = this->getLengthInBytes();
    if (lengthInBytes == 0 || output->bytesRemaining() < lengthInBytes) {
        return 1;
    }
    return do_copyTo(output);
}
