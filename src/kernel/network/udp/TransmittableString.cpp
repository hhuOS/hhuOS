//
// Created by hannes on 10.06.21.
//

#include "TransmittableString.h"

TransmittableString::TransmittableString(size_t length) {
    characters = new NetworkByteBlock(length);
}

TransmittableString::~TransmittableString() {
    delete characters;
}

void TransmittableString::append(char *chars, size_t length) {
    characters->append(chars,length);
}
