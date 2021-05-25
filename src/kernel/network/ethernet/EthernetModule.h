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
        Util::ArrayList<EthernetDevice *> *ethernetDevices;
    public:
        EthernetModule();

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

        Util::ArrayList<EthernetDevice *> *getEthernetDevices() const;

        void registerEthernetDevice(EthernetDevice *ethernetDevice);

        void unregisterEthernetDevice(EthernetDevice *ethernetDevice);
    };

};

#endif //HHUOS_ETHERNETMODULE_H
