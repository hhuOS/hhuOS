//
// Created by hannes on 19.04.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "Loopback.h"

Loopback::Loopback(Kernel::EventBus *eventBus) {
    this->eventBus = eventBus;
}

void Loopback::sendPacket(void *address, uint16_t length) {
    if (length == 0 || address == nullptr) {
        log.error("Invalid values for packet address and/or length");
        return;
    }
    if (eventBus == nullptr) {
        log.error("Could not send packet, event bus was null!");
        return;
    }

    //Outgoing EthernetFrames will be dropped after sending, so we need to copy our data first
    auto *byteBlock = new NetworkByteBlock(length);
    if(byteBlock->isNull()){
        log.error("Could not init byteBlock, discarding packet");
        free(address);
        return;
    }
    if(byteBlock->writeBytes(address,length)||
            byteBlock->getCurrentIndex()!=(length-1)){
        log.error("Could not copy incoming data to byteBlock, discarding packet");
        delete byteBlock;
        free(address);
        return;
    }
    eventBus->publish(
            Util::SmartPointer<Kernel::Event>(
                    new Kernel::ReceiveEvent(byteBlock->getBytes(), length)
            )
    );
}

void Loopback::getMacAddress(uint8_t *buf) {
    if (buf == nullptr) {
        log.error("No valid buffer for MAC address given! Exit");
        return;
    }
    for (int i = 0; i < 6; i++) {
        buf[i] = 0;
    }
}

void Loopback::trigger(Kernel::InterruptFrame &frame) {
//TODO: Implement this one
}
