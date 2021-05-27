//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETMODULE_H
#define HHUOS_ETHERNETMODULE_H

#include <kernel/log/Logger.h>
#include <kernel/event/Receiver.h>
#include "EthernetDevice.h"

namespace Kernel {

    class EthernetModule : public Receiver {
    private:
        uint8_t deviceCounter;
        Kernel::EventBus *eventBus;
        Util::HashMap<String *, EthernetDevice *> *ethernetDevices;
    public:
        EthernetModule(Kernel::EventBus *eventBus);

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

        EthernetDevice *getEthernetDevice(String *identifier);

        EthernetDevice *getEthernetDevice(NetworkDevice *pDevice);

        void registerNetworkDevice(NetworkDevice *networkDevice);

        void registerNetworkDevice(String *identifier, NetworkDevice *networkDevice);

        void unregisterNetworkDevice(NetworkDevice *networkDevice);

        void collectEthernetDeviceAttributes(Util::ArrayList<String> *strings);
    };

};

#endif //HHUOS_ETHERNETMODULE_H
