/*
 * Copyright (C) 2018/19 Thiemo Urselmann
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
 *
 * Note:
 * All references marked with [...] refer to the following developers manual.
 * Intel Corporation. PCI/PCI-X Family of Gigabit Ethernet Controllers Software Developer’s Manual.
 * 317453006EN.PDF Revision 4.0. 2009.
 */

#ifndef _Intel82541IP_include_
#define _Intel82541IP_include_


#include "device/network/e1000/E1000.h"
/**
 * This class implements the abstract class E1000.
 *
 * It will be initialized in the E1000::initialize method.
 * This drivers setup method calls it.
 * Because the corresponding card has an EEPROM, the MAC-address
 * will be read from it and stored in the Mac attribute from E1000.
 *
 * All methods called due proceeding E1000::initialize are implemented
 * in this class.
 */
class Intel82541IP final : public E1000 {
public:
    /**
     * An class for reading the MAC-address from.
     */
    EepromRead *eeprom = nullptr;

    /**
     * A log for providing logging information on the kernel log.
     */
    Logger &log = Logger::get("Intel82541IP");


    Intel82541IP() = default;
    ~Intel82541IP() override = default;

    /**
     * @return An instance of this driver.
     */
    PCI_DEVICE_DRIVER_IMPLEMENT_CREATE_INSTANCE(Intel82541IP);

    /**
     * Inherited methods from E1000.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

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

    Util::Array<Util::Pair<uint16_t, uint16_t>> getIdPairs() const final;
};

#endif
