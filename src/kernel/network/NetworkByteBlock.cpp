//
// Created by hannes on 30.05.21.
//

#include <lib/libc/printf.h>
#include <netinet/in.h>
#include "NetworkByteBlock.h"

//Constructor for blank ByteBlock to be filled
NetworkByteBlock::NetworkByteBlock(size_t length) {
    this->bytes = new uint8_t[length];
    this->length = length;

    //Cleanup memory
    for (size_t i = 0; i < length; i++) {
        bytes[i] = 0;
    }
}

//Constructor for given input to be parsed later
NetworkByteBlock::NetworkByteBlock(void *packet, size_t length) {
    this->bytes = new uint8_t [length];
    this->length=length;

    append(packet, length);

    this->currentIndex=0;
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

size_t NetworkByteBlock::getLength() const {
    return length;
}

void NetworkByteBlock::printBytes() {
    if (this->bytes == nullptr) {
        return;
    }
    printf("\nBytes: ");
    for (size_t i = 0; i < length; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n\n");
}

uint8_t NetworkByteBlock::append(uint8_t oneByte) {
    return append(&oneByte, sizeof oneByte);
}

uint8_t NetworkByteBlock::append(uint16_t twoBytes) {
    uint16_t switchedBytes = htons(twoBytes);
    return append(&switchedBytes, sizeof twoBytes);
}

uint8_t NetworkByteBlock::append(uint32_t fourBytes) {
    uint32_t switchedBytes = htons(fourBytes);
    return append(&switchedBytes, sizeof fourBytes);
}

uint8_t NetworkByteBlock::append(void *source, size_t byteCount) {
    auto *source = (uint8_t *) source;
    //Avoid writing beyond last byte
    if (this->bytes == nullptr || (this->currentIndex + byteCount) > this->length) {
        return 1;
    }
    if (byteCount == 0) {
        //It's not an error if nothing needs to be done...
        return 0;
    }
    for (size_t i = 0; i < byteCount; i++) {
        this->bytes[currentIndex + i] = source[i];
    }
    this->currentIndex += byteCount;
    return 0;
}

uint8_t NetworkByteBlock::read(uint8_t *oneByte) {
    return read(oneByte, sizeof oneByte);
}

uint8_t NetworkByteBlock::read(uint16_t *twoBytes) {
    uint16_t tempValue = 0;
    if(read(&tempValue, sizeof tempValue)){
        return 1;
    }
    *twoBytes= ntohs(tempValue);
    return 0;
}

uint8_t NetworkByteBlock::read(uint32_t *fourBytes) {
    uint32_t tempValue = 0;
    if(read(&tempValue, sizeof tempValue)){
        return 1;
    }
    *fourBytes= ntohs(tempValue);
    return 0;
}

uint8_t NetworkByteBlock::read(void *target, size_t byteCount) {
    if (
            this->currentIndex == this->length ||
            byteCount == 0 ||
            target == nullptr ||
            this->bytes == nullptr ||
            this->currentIndex + byteCount > this->length
            ) {
        return 1;
    }
    auto *targetBytes = (uint8_t *) target;
    for (size_t i = 0; i < byteCount; i++) {
        targetBytes[i] = this->bytes[currentIndex + i];
    }
    currentIndex += byteCount;
    return 0;
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

size_t NetworkByteBlock::bytesRemaining() const {
    return this->length - this->currentIndex;
}
