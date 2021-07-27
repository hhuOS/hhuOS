//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4MODULE_H
#define HHUOS_ICMP4MODULE_H

#include <kernel/log/Logger.h>
#include <kernel/service/EventBus.h>
#include <kernel/network/internet/icmp/ICMP4Message.h>
#include <kernel/network/internet/icmp/ICMP4Echo.h>
#include <kernel/network/internet/icmp/ICMP4EchoReply.h>
#include <kernel/network/NetworkDefinitions.h>
#include <kernel/service/TimeService.h>

namespace Kernel {
    class ICMP4Module : public Receiver {
    private:
        EventBus *eventBus = nullptr;
        TimeService *timeService = nullptr;

        Spinlock *accessLock = nullptr;
        uint32_t pingTimes[ICMP_PING_BUFFER_SIZE]{};

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("ICMP4Module");

        uint8_t processICMP4Message(IP4Header *ip4Header, NetworkByteBlock *input);

        uint8_t startPingTime(ICMP4Echo *echoMessage);

        uint8_t stopPingTime(uint32_t *resultTarget, ICMP4EchoReply *echoReplyMessage);

    public:

        explicit ICMP4Module(EventBus *eventBus);

/**
     * Inherited method from Receiver.
     * This method is meant to be overridden and
     * implemented by this class.
     */
        void onEvent(const Event &event) override;
    };
}

#endif //HHUOS_ICMP4MODULE_H
