//
// Created by hannes on 30.05.21.
//

#include <lib/libc/printf.h>
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
        delete[] this->bytes;
    }

//If our byteBlock is perfectly filled, this->currentIndex points to the first 'illegal' byte
// => (this->currentIndex+byteCount) is exactly equal this->length then!
    bool NetworkByteBlock::isCompletelyFilled() const {
        return this->currentIndex == this->length;
    }

    size_t NetworkByteBlock::getLength() const {
        return length;
    }

    uint8_t NetworkByteBlock::appendOneByte(uint8_t oneByte) {
        return appendStraightFrom(&oneByte, sizeof oneByte);
    }

    uint8_t NetworkByteBlock::appendTwoBytesSwapped(uint16_t twoBytes) {
        auto *twoBytesAsArray = (uint8_t *) &twoBytes;
        uint8_t switchedBytes[sizeof twoBytes] =
                {twoBytesAsArray[1], twoBytesAsArray[0]};
        return appendStraightFrom(switchedBytes, sizeof twoBytes);
    }

    uint8_t NetworkByteBlock::appendStraightFrom(NetworkByteBlock *otherByteBlock, size_t byteCount) {
        //Return error if we can't read all bytes from the other byteBlock
        if (otherByteBlock == nullptr || byteCount > otherByteBlock->bytesRemaining()) {
            return 1;
        }
        //The other byteBlock has the same type
        //-> we can access its internal attributes here!
        return appendStraightFrom(otherByteBlock->bytes, byteCount);
    }

    uint8_t NetworkByteBlock::appendStraightFrom(void *source, size_t byteCount) {
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

    uint8_t NetworkByteBlock::readOneByteTo(uint8_t *oneByte) {
        if (oneByte == nullptr) {
            return 1;
        }
        return readStraightTo(oneByte, sizeof *oneByte);
    }

    uint8_t NetworkByteBlock::readTwoBytesSwappedTo(uint16_t *twoBytes) {
        if (twoBytes == nullptr) {
            return 1;
        }
        auto *twoBytesAsArray = (uint8_t *) twoBytes;

        readOneByteTo(&twoBytesAsArray[1]);
        readOneByteTo(&twoBytesAsArray[0]);
        return 0;
    }

    uint8_t NetworkByteBlock::readStraightTo(void *target, size_t byteCount) {
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

    size_t NetworkByteBlock::bytesRemaining() const {
        return this->length - this->currentIndex;
    }

    uint8_t NetworkByteBlock::decrementIndex() {
        //Avoid breakout
        if (currentIndex > 0) {
            currentIndex--;
        }
        return 0;
    }

    uint8_t NetworkByteBlock::resetIndex() {
        currentIndex = 0;
        return 0;
    }

    String NetworkByteBlock::asString(size_t startIndex, size_t endIndex, size_t bytesPerLine) {
        String byteString = "";
        if (this->bytes == nullptr) {
            return byteString;
        }
        for (size_t i = startIndex; (i < length) && (i < endIndex); i++) {
            if (((i - startIndex) % bytesPerLine) == 0) {
                if ((i - startIndex) > 0) {
                    byteString += "\n";
                }
            }
            byteString += String::format("%02x ", bytes[i]);
        }
        return byteString;
    }
}