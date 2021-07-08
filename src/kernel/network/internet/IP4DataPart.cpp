//
// Created by hannes on 08.07.21.
//

#include "IP4DataPart.h"

IP4DataPart::~IP4DataPart() = default;


uint8_t IP4DataPart::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }
    size_t lengthInBytes = this->getLengthInBytes();
    if (lengthInBytes == 0 || output->bytesRemaining() < lengthInBytes) {
        return 1;
    }
    return do_copyTo(output);
}
