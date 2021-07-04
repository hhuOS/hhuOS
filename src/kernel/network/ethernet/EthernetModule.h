//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETMODULE_H
#define HHUOS_ETHERNETMODULE_H

#include <kernel/core/Management.h>
#include "EthernetDevice.h"

namespace Kernel {
    class EthernetModule : public Receiver {
    private:
        uint8_t deviceCounter = 0;
        Spinlock *accessLock = nullptr;
        EventBus *eventBus = nullptr;
        EthernetAddress *broadcastAddress = nullptr;
        Util::ArrayList<EthernetDevice *> *devices = nullptr;

        bool isForUsOrBroadcast(EthernetHeader *ethernetHeader);

    public:

        explicit EthernetModule(EventBus *eventBus);

        ~EthernetModule() override;

        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("EthernetModule");

/**
     * Inherited method from Receiver.
     * This method is meant to be overridden and
     * implemented by this class.
     */
        void onEvent(const Event &event) override;

        EthernetDevice *getEthernetDevice(const String &identifier);

        EthernetDevice *getEthernetDevice(NetworkDevice *networkDevice);

        uint8_t registerNetworkDevice(NetworkDevice *networkDevice, uint8_t *sendBuffer, void *physicalBufferAddress);

        uint8_t registerNetworkDevice(const String &identifier, NetworkDevice *networkDevice, uint8_t *sendBuffer,
                                      void *physicalBufferAddress);

        uint8_t unregisterNetworkDevice(NetworkDevice *networkDevice);

        uint8_t collectEthernetDeviceAttributes(Util::ArrayList<String> *strings);
    };

}

#endif //HHUOS_ETHERNETMODULE_H
