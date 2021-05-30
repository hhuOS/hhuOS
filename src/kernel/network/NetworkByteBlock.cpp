//
// Created by hannes on 30.05.21.
//

#include "NetworkByteBlock.h"

NetworkByteBlock::NetworkByteBlock(size_t length) {
    this->bytes = (uint8_t *) malloc(length);
    this->length = length;
}

NetworkByteBlock::~NetworkByteBlock() {
    freeBytes();
}

bool NetworkByteBlock::isNull() {
    return this->bytes == nullptr;
}

void NetworkByteBlock::freeBytes() {
    if (!isNull()) {
        free(this->bytes);
        this->bytes = nullptr;
    }
}

uint8_t NetworkByteBlock::writeBytes(void *memoryAddress, size_t byteCount) {
    //Avoid writing beyond last byte
    if (isNull() || (this->currentIndex + byteCount) >= this->length) {
        return 1;
    }
    if (byteCount == 0) {
        //It's not an error if nothing needs to be done...
        return 0;
    }
    memcpy(this->bytes, memoryAddress, byteCount);
    this->currentIndex += byteCount;
    return 0;
}

void *NetworkByteBlock::getBytes() {
    return (void *) this->bytes;
}

size_t NetworkByteBlock::getCurrentIndex() const {
    return currentIndex;
}
