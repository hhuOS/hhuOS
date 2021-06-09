//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4MODULE_H
#define HHUOS_ICMP4MODULE_H

#include <kernel/log/Logger.h>
#include <kernel/network/NetworkEventBus.h>

namespace Kernel {

    class ICMP4Module : public Receiver {
    private:
        NetworkEventBus *eventBus;
        typedef struct ip4information {
            uint8_t version_headerLength = 0;

            //standard type of service, no priority etc.
            uint8_t typeOfService = 0;

            uint16_t totalLength = 0;

            //fragmentation not used here, fragment parameters not set
            uint16_t identification = 0;
            uint16_t flags_fragmentOffset = 0;

            //solid default value, can be set from constructor if necessary
            uint8_t timeToLive = 0;

            uint8_t protocolType = 0;
            uint16_t headerChecksum = 0;
            uint8_t sourceAddress[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
            uint8_t destinationAddress[IP4ADDRESS_LENGTH]{0, 0, 0, 0};
        } ip4info_t;

        ip4info_t info;
    public:
        explicit ICMP4Module(NetworkEventBus *eventBus);

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("ICMP4Module");


/**
     * Inherited method from Receiver.
     * This method is meant to be overridden and
     * implemented by this class.
     */
        void onEvent(const Event &event) override;
    };
}

#endif //HHUOS_ICMP4MODULE_H
