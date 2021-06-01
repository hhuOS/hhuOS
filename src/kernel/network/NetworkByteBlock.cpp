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
        delete (uint8_t *) this->bytes;
        this->bytes = nullptr;
    }
}

uint8_t NetworkByteBlock::appendBytesInNetworkByteOrder(void *memoryAddress, size_t byteCount) {
    auto *source=(uint8_t *)memoryAddress;
    //Avoid writing beyond last byte
    if (this->bytes== nullptr || (this->currentIndex + byteCount) > this->length) {
        return 1;
    }
    if (byteCount == 0) {
        //It's not an error if nothing needs to be done...
        return 0;
    }
    //Write bytes in switched order to convert data to BigEndian, as we are running on LittleEndian here
    //-> RFC1700 page 3 defines BigEndian as byte order for communication via Internet Protocols
    for(size_t i=0;i<byteCount;i++){
        this->bytes[currentIndex+i]=source[byteCount-1-i];
    }
    this->currentIndex += byteCount;
    return 0;
}

size_t NetworkByteBlock::getLength() const {
    return length;
}

uint8_t NetworkByteBlock::sendOutVia(NetworkDevice *networkDevice) {
    if (this->length == 0) {
        //It's not an error if nothing needs to be done
        return 0;
    }
    if (networkDevice == nullptr || this->isNull()) {
        return 1;
    }
    networkDevice->sendPacket(bytes, length);
    return 0;
}
