//
// Created by hannes on 30.05.21.
//

#include "NetworkByteBlock.h"

namespace Kernel {
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
        if (otherByteBlock== nullptr || byteCount > otherByteBlock->bytesRemaining()) {
            return 1;
        }
        //The other byteBlock has the same type
        //-> we can access its internal attributes here!
        return append(otherByteBlock->bytes, byteCount);
    }

    uint8_t NetworkByteBlock::append(void *source, size_t byteCount) {
        //Avoid writing beyond last byte
        if (
                source == nullptr ||
                this->bytes == nullptr ||
                (this->currentIndex + byteCount) > this->length
            ) {
            return 1;
        }
        if (byteCount == 0) {
            //It's not an error if nothing needs to be done...
            return 0;
        }
        auto *sourceAsBytes = (uint8_t *) source;
        for (size_t i = 0; i < byteCount; i++) {
            this->bytes[currentIndex + i] = sourceAsBytes[i];
        }
        this->currentIndex += byteCount;
        return 0;
    }

    uint8_t NetworkByteBlock::read(uint8_t *oneByte) {
        if(oneByte== nullptr){
            return 1;
        }
        return read(oneByte, 1);
    }

    uint8_t NetworkByteBlock::read(uint16_t *twoBytes) {
        if(twoBytes== nullptr){
            return 1;
        }
        auto *twoBytesAsArray = (uint8_t *) twoBytes;

        read(&twoBytesAsArray[1]);
        read(&twoBytesAsArray[0]);
        return 0;
    }

    uint8_t NetworkByteBlock::read(uint32_t *fourBytes) {
        if(fourBytes== nullptr){
            return 1;
        }
        auto *fourBytesAsArray = (uint8_t *) fourBytes;

        read(&fourBytesAsArray[3]);
        read(&fourBytesAsArray[2]);
        read(&fourBytesAsArray[1]);
        read(&fourBytesAsArray[0]);
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
        if (networkDevice == nullptr || bytes == nullptr) {
            return 1;
        }

        if (this->length == 0) {
            //It's not an error if nothing needs to be done
            return 0;
        }
        networkDevice->sendPacket(bytes, static_cast<uint16_t>(length));
        //NetworkDevices return no information about errors or successful sending
        //-> we just can return successful here
        return 0;
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
        return 0;
    }

    uint8_t NetworkByteBlock::decrementIndex() {
        currentIndex--;
        return 0;
    }

    uint8_t NetworkByteBlock::resetIndex() {
        currentIndex = 0;
        return 0;
    }
}