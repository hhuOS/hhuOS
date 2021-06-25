//
// Created by hannes on 30.05.21.
//

#ifndef HHUOS_NETWORKBYTEBLOCK_H
#define HHUOS_NETWORKBYTEBLOCK_H

#include <device/network/NetworkDevice.h>

namespace Kernel {
    class NetworkByteBlock {
    private:
        uint8_t *bytes = nullptr;
        size_t length = 0, currentIndex = 0;

    public:
        explicit NetworkByteBlock(size_t length);

        virtual ~NetworkByteBlock();

        [[nodiscard]] size_t getLength() const;

        [[nodiscard]] bool isCompletelyFilled() const;

        [[nodiscard]] size_t bytesRemaining() const;

        uint8_t appendOneByte(uint8_t oneByte);

        uint8_t appendTwoBytesSwapped(uint16_t twoBytes);

        uint8_t appendStraightFrom(void *source, size_t byteCount);

        uint8_t appendStraightFrom(NetworkByteBlock *otherByteBlock, size_t byteCount);

        uint8_t readOneByteTo(uint8_t *oneByte);

        uint8_t readTwoBytesSwappedTo(uint16_t *twoBytes);

        uint8_t readStraightTo(void *target, size_t byteCount);

        uint8_t decrementIndex();

        uint8_t resetIndex();

        [[maybe_unused]] void printBytes();
    };
}

#endif //HHUOS_NETWORKBYTEBLOCK_H
