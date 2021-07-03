//
// Created by hannes on 19.04.21.
//

#include <kernel/network/NetworkByteBlock.h>
#include "Loopback.h"

namespace Kernel {
    Loopback::Loopback(EventBus *eventBus) {
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
        auto receivePacketEvent =
                Util::SmartPointer<Event>(new ReceiveEvent(address, length));
        eventBus->publish(
                //our outgoing EthernetFrame will be dropped afterwards,
                //but a ReceiveEvent copies incoming data into a separate array,
                // so no need to copy data here

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

    void Loopback::trigger(InterruptFrame &frame) {
//TODO: Implement this one
    }
}