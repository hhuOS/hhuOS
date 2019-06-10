/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef HHUOS_INTEL82540EM_H
#define HHUOS_INTEL82540EM_H

#include "device/network/e1000/E1000.h"
#include "TransmitDescriptor82540EM.h"

class Intel82540EM : public E1000 {

public:

    /**
     * Constructor.
     */
    Intel82540EM() = default;

    /**
     * Destructor.
     */
    ~Intel82540EM() override = default;

    PCI_DEVICE_DRIVER_IMPLEMENT_CREATE_INSTANCE(Intel82540EM);

    Util::Array<Util::Pair<uint16_t, uint16_t>> getIdPairs() const final;

    void setup(const Pci::Device &dev) final;

    Descriptors<TransmitDescriptor *> *createTransmitDescriptorBlock(uint8_t *buffer) final;

    Descriptors<ReceiveDescriptor *> *createReceiveDescriptorBlock(uint8_t *buffer) final;

    void initializeAttributeClasses(Descriptors<TransmitDescriptor *> *transmitDescriptors,
                                    Descriptors<ReceiveDescriptor *> *receiveDescriptors, uint64_t phyTransmitBlock,
                                    uint64_t phyReceiveBlock) final;

    void setDeviceControl(DeviceControl *control) final;

    void setInterruptControl(InterruptEnable *enable, InterruptDisable *disable) final;

    void setTransmitControl(TransmitControl *control) final;

    void setReceiveControl(ReceiveControl *control) final;

    void trigger(InterruptFrame &frame) final;

    void loadMac() final;

    void plugin() final;

    void createNodes() final;

private:

    EepromRead *eeprom = nullptr;

    Logger &log = Logger::get("Intel82540EM");

};

#endif
