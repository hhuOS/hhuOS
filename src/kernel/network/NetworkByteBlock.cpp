//
// Created by hannes on 30.05.21.
//

#include "NetworkByteBlock.h"

NetworkByteBlock::NetworkByteBlock(size_t length) {
    this->bytes = new uint8_t[length];
    this->length = length;
}

NetworkByteBlock::~NetworkByteBlock() {
    freeBytes();
}

bool NetworkByteBlock::isNull() {
    return this->bytes == nullptr;
}

//If our byteBlock is perfectly filled, this->currentIndex points to the first 'illegal' byte
// => (this->currentIndex+byteCount) is exactly equal this->length then!
bool NetworkByteBlock::isCompletelyFilled() const {
    return this->currentIndex == this->length;
}

void NetworkByteBlock::freeBytes() {
    if (!isNull()) {
        delete (uint8_t *)this->bytes;
        this->bytes = nullptr;
    }
}

uint8_t NetworkByteBlock::writeBytes(void *memoryAddress, size_t byteCount) {
    //Avoid writing beyond last byte
    if (isNull() || (this->currentIndex + byteCount) > this->length) {
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

size_t NetworkByteBlock::getLength() const {
    return length;
}

uint8_t NetworkByteBlock::sendOutVia(NetworkDevice *networkDevice) {
    if(this->length==0){
        //It's not an error if nothing needs to be done
        return 0;
    }
    if(networkDevice== nullptr||this->isNull()){
        return 1;
    }
    networkDevice->sendPacket(bytes,length);
    return 0;
}
