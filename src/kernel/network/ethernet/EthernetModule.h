//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETMODULE_H
#define HHUOS_ETHERNETMODULE_H

#include <kernel/network/NetworkEventBus.h>
#include <kernel/core/Management.h>
#include "EthernetDevice.h"
#include "EthernetDeviceIdentifier.h"

namespace Kernel {
    class EthernetModule : public Receiver {
    private:
        uint8_t deviceCounter = 0;
        Spinlock *accessLock = nullptr;
        NetworkEventBus *eventBus = nullptr;
        Management *systemManagement = nullptr;
        EthernetAddress *broadcastAddress = nullptr;
        EthernetDeviceIdentifier *loopbackIdentifier = nullptr;
        Util::ArrayList<EthernetDevice *> *ethernetDevices = nullptr;

        void deleteSendBuffer(const EthernetDevice *ethernetDevice);

        bool isForUs(EthernetHeader *ethernetHeader);

    public:

        explicit EthernetModule(Management *systemManagement, NetworkEventBus *eventBus,
                                EthernetDeviceIdentifier *loopbackIdentifier);

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

        EthernetDevice *getEthernetDevice(EthernetDeviceIdentifier *identifier);

        EthernetDevice *getEthernetDevice(NetworkDevice *networkDevice);

        void registerNetworkDevice(NetworkDevice *networkDevice);

        uint8_t registerNetworkDevice(EthernetDeviceIdentifier *identifier, NetworkDevice *networkDevice);

        uint8_t unregisterNetworkDevice(NetworkDevice *networkDevice);

        uint8_t collectEthernetDeviceAttributes(Util::ArrayList<String> *strings);

        ~EthernetModule() override;
    };

}

#endif //HHUOS_ETHERNETMODULE_H
