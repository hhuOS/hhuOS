//
// Created by hannes on 30.05.21.
//

#ifndef HHUOS_NETWORKBYTEBLOCK_H
#define HHUOS_NETWORKBYTEBLOCK_H

#include <device/network/NetworkDevice.h>
#include <kernel/event/network/ReceiveEvent.h>

class NetworkByteBlock {
private:
    uint8_t *bytes = nullptr;
    size_t length, currentIndex = 0;

public:

    NetworkByteBlock(void *packet, size_t length);

    NetworkByteBlock(size_t length);

    virtual ~NetworkByteBlock();

    size_t getLength() const;

    bool isNull();

    bool isCompletelyFilled() const;

    void printBytes();

    void freeBytes();

    size_t bytesRemaining() const;

    uint8_t sendOutVia(NetworkDevice *pDevice);

    uint8_t append(uint8_t oneByte);

    uint8_t append(uint16_t twoBytes);

    uint8_t append(uint32_t fourBytes);

    uint8_t append(void *source, size_t byteCount);

    uint8_t read(uint8_t *oneByte);

    uint8_t read(uint16_t *twoBytes);

    uint8_t read(uint32_t *fourBytes);

    uint8_t read(void *target, size_t byteCount);
};


#endif //HHUOS_NETWORKBYTEBLOCK_H
