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
    NetworkByteBlock(size_t length);

    size_t getLength() const;

    bool isNull();

    bool isCompletelyFilled() const;

    void freeBytes();

    virtual ~NetworkByteBlock();

    uint8_t appendBytesInNetworkByteOrder(void *memoryAddress, size_t byteCount);

    uint8_t sendOutVia(NetworkDevice *pDevice);

    void printBytes();
};


#endif //HHUOS_NETWORKBYTEBLOCK_H
