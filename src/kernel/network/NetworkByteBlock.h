//
// Created by hannes on 30.05.21.
//

#ifndef HHUOS_NETWORKBYTEBLOCK_H
#define HHUOS_NETWORKBYTEBLOCK_H

#include <cstdint>
#include <lib/string/String.h>

namespace Kernel {
    class NetworkByteBlock {
    private:
        uint8_t *bytes = nullptr;
        uint16_t length = 0, currentIndex = 0;

    public:
        explicit NetworkByteBlock(uint16_t length);

        virtual ~NetworkByteBlock();

        [[nodiscard]] uint16_t getLength() const;

        [[nodiscard]] uint16_t bytesRemaining() const;

        uint8_t appendOneByte(uint8_t oneByte);

        uint8_t appendTwoBytesSwapped(uint16_t twoBytes);

        uint8_t appendStraightFrom(void *source, uint16_t byteCount);

        uint8_t appendStraightFrom(NetworkByteBlock *otherByteBlock, uint16_t byteCount);

        uint8_t readOneByteTo(uint8_t *oneByte);

        uint8_t readTwoBytesSwappedTo(uint16_t *twoBytes);

        uint8_t readStraightTo(void *target, uint16_t byteCount);

        uint8_t decrementIndex();

        uint8_t resetIndex();

        String asString(uint16_t startIndex, uint16_t endIndex, uint16_t bytesPerLine);
    };
}

#endif //HHUOS_NETWORKBYTEBLOCK_H
