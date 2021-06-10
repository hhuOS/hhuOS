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

void TransmittableString::append(void *chars, size_t length) {
    characters->append(chars,length);
}

uint8_t TransmittableString::copyTo(NetworkByteBlock *byteBlock) {
    //Reset index or it already is at the end and breaks copy to another ByteBlock!
    characters->decreaseIndex(characters->getLength());
    return byteBlock->append(characters,characters->getLength());
}

size_t TransmittableString::getLengthInBytes() {
    return characters->getLength();
}

uint8_t TransmittableString::parseData(NetworkByteBlock *input) {
    if(characters->getCurrentIndex()>0){
        return 1;
    }
    return characters->append(input, input->getLength());
}

void TransmittableString::copyTo(void *target, size_t byteCount) {
    characters->decreaseIndex(characters->getLength());
    characters->read(target,byteCount);
}
