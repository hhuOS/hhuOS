//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETMODULE_H
#define HHUOS_ETHERNETMODULE_H

#include <kernel/core/System.h>
#include <kernel/log/Logger.h>
#include <kernel/event/Receiver.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include <kernel/event/network/IP4ReceiveEvent.h>
#include <kernel/event/network/ARPReceiveEvent.h>
#include <kernel/network/NetworkEventBus.h>

#include "EthernetDevice.h"

#define ETH_DELIVER_SUCCESS 0
#define ETH_FRAME_NULL 21
#define ETH_DEVICE_NULL 22
#define ETH_COPY_BYTEBLOCK_FAILED 23
#define ETH_COPY_BYTEBLOCK_INCOMPLETE 24

namespace Kernel {
    class EthernetModule : public Receiver {
    private:
        uint8_t deviceCounter = 0;
        NetworkEventBus *eventBus = nullptr;
        Util::HashMap<String *, EthernetDevice *> *ethernetDevices = nullptr;
    public:
        explicit EthernetModule(NetworkEventBus *eventBus);

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

        EthernetDevice *getEthernetDevice(NetworkDevice *networkDevice);

        void registerNetworkDevice(NetworkDevice *networkDevice);

        void registerNetworkDevice(String *identifier, NetworkDevice *networkDevice);

        void unregisterNetworkDevice(NetworkDevice *networkDevice);

        void collectEthernetDeviceAttributes(Util::ArrayList<String> *strings);
    };

};

#endif //HHUOS_ETHERNETMODULE_H
