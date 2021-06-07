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

NetworkByteBlock::~NetworkByteBlock() {
    delete this->bytes;
}

//If our byteBlock is perfectly filled, this->currentIndex points to the first 'illegal' byte
// => (this->currentIndex+byteCount) is exactly equal this->length then!
bool NetworkByteBlock::isCompletelyFilled() const {
    return this->currentIndex == this->length;
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
    auto *twoBytesAsArray = (uint8_t *) &twoBytes;
    uint8_t switchedBytes[2] = {twoBytesAsArray[1], twoBytesAsArray[0]};
    return append(switchedBytes, sizeof twoBytes);
}

uint8_t NetworkByteBlock::append(uint32_t fourBytes) {
    auto *fourBytesAsArray = (uint8_t *) &fourBytes;
    uint8_t switchedBytes[4] = {fourBytesAsArray[3], fourBytesAsArray[2], fourBytesAsArray[1], fourBytesAsArray[0]};
    return append(switchedBytes, sizeof fourBytes);
}

uint8_t NetworkByteBlock::append(NetworkByteBlock *otherByteBlock, size_t byteCount) {
    //Return error if we can't read all bytes from the other byteBlock
    if (byteCount > otherByteBlock->bytesRemaining()) {
        return 1;
    }
    //The other byteBlock has the same type
    //-> we can access its internal attributes here!
    return append(otherByteBlock->bytes, byteCount);
}

uint8_t NetworkByteBlock::append(void *source, size_t byteCount) {
    auto *sourceAsByteArray = (uint8_t *) source;
    //Avoid writing beyond last byte
    if (this->bytes == nullptr || (this->currentIndex + byteCount) > this->length) {
        return 1;
    }
    if (byteCount == 0) {
        //It's not an error if nothing needs to be done...
        return BYTEBLOCK_ACTION_SUCCESS;
    }
    for (size_t i = 0; i < byteCount; i++) {
        this->bytes[currentIndex + i] = sourceAsByteArray[i];
    }
    this->currentIndex += byteCount;
    return BYTEBLOCK_ACTION_SUCCESS;
}

uint8_t NetworkByteBlock::read(uint8_t *oneByte) {
    return read(oneByte, sizeof oneByte);
}

uint8_t NetworkByteBlock::read(uint16_t *twoBytes) {
    uint16_t tempValue = 0;
    if (read(&tempValue, sizeof tempValue)) {
        return 1;
    }
    auto *tempValueAsArray = (uint8_t *) &tempValue;
    auto *twoBytesAsArray = (uint8_t *) twoBytes;

    twoBytesAsArray[0] = tempValueAsArray[1];
    twoBytesAsArray[1] = tempValueAsArray[0];
    return BYTEBLOCK_ACTION_SUCCESS;
}

uint8_t NetworkByteBlock::read(uint32_t *fourBytes) {
    uint32_t tempValue = 0;
    if (read(&tempValue, sizeof tempValue)) {
        return 1;
    }
    auto *tempValueAsArray = (uint8_t *) &tempValue;
    auto *fourBytesAsArray = (uint8_t *) fourBytes;

    fourBytesAsArray[0] = tempValueAsArray[3];
    fourBytesAsArray[1] = tempValueAsArray[2];
    fourBytesAsArray[2] = tempValueAsArray[1];
    fourBytesAsArray[3] = tempValueAsArray[0];
    return BYTEBLOCK_ACTION_SUCCESS;
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
    return BYTEBLOCK_ACTION_SUCCESS;
}

uint8_t NetworkByteBlock::sendOutVia(NetworkDevice *networkDevice) {
    if (networkDevice == nullptr) {
        return BYTEBLOCK_NETWORK_DEVICE_NULL;
    }

    if (bytes == nullptr) {
        return BYTEBLOCK_BYTES_NULL;
    }

    if (this->length == 0) {
        //It's not an error if nothing needs to be done
        return BYTEBLOCK_ACTION_SUCCESS;
    }
    networkDevice->sendPacket(bytes, static_cast<uint16_t>(length));
    //NetworkDevices return no information about errors or successful sending
    //-> we just can return successful here
    return BYTEBLOCK_ACTION_SUCCESS;
}

size_t NetworkByteBlock::bytesRemaining() const {
    return this->length - this->currentIndex;
}

uint8_t NetworkByteBlock::skip(size_t byteCount) {
    if (
            this->currentIndex == this->length ||
            byteCount == 0 ||
            this->bytes == nullptr ||
            this->currentIndex + byteCount > this->length
            ) {
        return 1;
    }
    //No problem if byteCount == 0 here
    this->currentIndex += byteCount;
    return BYTEBLOCK_ACTION_SUCCESS;
}

uint8_t NetworkByteBlock::decrementIndex(size_t byteCount) {
    if(byteCount > currentIndex){
        return 1;
    }
    currentIndex-=byteCount;
    return 0;
}
