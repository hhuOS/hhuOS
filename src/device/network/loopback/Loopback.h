//
// Created by hannes on 19.04.21.
//

#ifndef HHUOS_LOOPBACK_H
#define HHUOS_LOOPBACK_H

#include <device/network/NetworkDevice.h>
#include <kernel/service/EventBus.h>

/**
 *
 */
class Loopback : public NetworkDevice {

public:
    /**
     * Overriding function from NetworkDevice.
     */
    void sendPacket(void *address, uint16_t length) override;

    /**
     * Overriding function from NetworkDevice.
     */
    void getMacAddress(uint8_t *buf) override;
};

#endif //HHUOS_LOOPBACK_H
