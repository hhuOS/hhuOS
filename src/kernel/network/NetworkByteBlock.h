//
// Created by hannes on 30.05.21.
//

#ifndef HHUOS_NETWORKBYTEBLOCK_H
#define HHUOS_NETWORKBYTEBLOCK_H

#include <device/network/NetworkDevice.h>
#include <kernel/event/network/ReceiveEvent.h>

#define BYTEBLOCK_ACTION_SUCCESS 0
#define BYTEBLOCK_NETWORK_DEVICE_NULL 30
#define BYTEBLOCK_BYTES_NULL 31

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

    uint8_t sendOutVia(NetworkDevice *pDevice);

    uint8_t append(uint8_t oneByte);

    uint8_t append(uint16_t twoBytes);

    uint8_t append(uint32_t fourBytes);

    uint8_t append(const uint8_t *source, size_t byteCount);

    uint8_t append(NetworkByteBlock *otherByteBlock, size_t byteCount);

    uint8_t read(uint8_t *oneByte);

    uint8_t read(uint16_t *twoBytes);

    uint8_t read(uint32_t *fourBytes);

    uint8_t read(uint8_t *target, size_t byteCount);

    uint8_t skip(size_t byteCount);

    uint8_t decreaseIndex(size_t byteCount);

    uint8_t resetIndex();
};


#endif //HHUOS_NETWORKBYTEBLOCK_H
