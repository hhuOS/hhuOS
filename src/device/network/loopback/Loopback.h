//
// Created by hannes on 19.04.21.
//

#ifndef HHUOS_LOOPBACK_H
#define HHUOS_LOOPBACK_H

#include <kernel/event/network/ReceiveEvent.h>
#include <kernel/service/EventBus.h>
#include <kernel/core/System.h>
#include <kernel/log/Logger.h>

#include <lib/util/SmartPointer.h>

#include <device/network/NetworkDevice.h>
#include <kernel/network/internet/addressing/IP4Address.h>
#include <kernel/network/internet/addressing/IP4Netmask.h>
#include <kernel/network/NetworkEventBus.h>

/**
 *
 */
class Loopback : public NetworkDevice {

private:

    /**
     * A logger to provide logging information on the kernel log.
     */
    Kernel::Logger &log = Kernel::Logger::get("Loopback");

    Kernel::NetworkEventBus *eventBus = nullptr;

public:
    Loopback(Kernel::NetworkEventBus *eventBus);

    /**
     * Overriding function from NetworkDevice.
     */
    void sendPacket(void *address, uint16_t length) override;

    /**
     * Overriding function from NetworkDevice.
     */
    void getMacAddress(uint8_t *buf) override;

    void trigger(Kernel::InterruptFrame &frame) override;
};

#endif //HHUOS_LOOPBACK_H
