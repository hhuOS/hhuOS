//
// Created by hannes on 21.06.21.
//

#ifndef HHUOS_ETHERNETVIRTUALDEVICE_H
#define HHUOS_ETHERNETVIRTUALDEVICE_H

#include <kernel/core/Management.h>
#include "EthernetDevice.h"

namespace Kernel {
    class EthernetVirtualDevice : public EthernetDevice {
    public:
        EthernetVirtualDevice(Management *systemManagement, EthernetDeviceIdentifier *identifier,
                              NetworkDevice *networkDevice);

        uint8_t sendEthernetFrame(EthernetFrame *ethernetFrame) override;

        ~EthernetVirtualDevice() override;
    };
}

#endif //HHUOS_ETHERNETVIRTUALDEVICE_H
