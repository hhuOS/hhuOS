//
// Created by hannes on 08.07.21.
//

#include "IP4DataPart.h"

IP4DataPart::~IP4DataPart() = default;


uint8_t IP4DataPart::copyTo(Kernel::NetworkByteBlock *output) {
    if (output == nullptr || output->bytesRemaining() < this->getLengthInBytes()) {
        return 1;
    }
    return do_copyTo(output);
}
