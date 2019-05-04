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

#ifndef HHUOS_TRANSMITRINGSIMPLE_H
#define HHUOS_TRANSMITRINGSIMPLE_H

#include <lib/util/ArrayList.h>
#include "descriptor/legacy/TransmitDescriptor.h"
#include "devices/network/e1000/descriptorRing/HardwareDescriptorRing.h"
#include "devices/network/e1000/general/Descriptors.h"

#include "TransmitRing.h"
#include "TransmitControl.h"

/**
 * This class implements the TransmitRing interface.
 *
 * Outgoing packets will be send through the ethernet card
 * to another host.
 */
class TransmitRingSimple final : public  TransmitRing {
public:
    /**
     * Constructor.
     * @param descriptors The descriptors contained in the transmit descriptor block.
     * @param initialize The hardware descriptor ring initializer for transmit.
     */
    explicit TransmitRingSimple(Descriptors<TransmitDescriptor *> *descriptors, HardwareDescriptorRing *initialize);
    ~TransmitRingSimple() override = default;

private:
    /**
    * Descriptors of a coherent descriptor-block.
    */
    Descriptors<TransmitDescriptor *> *descriptors;

    /**
     * Used to initialize the ring.
     */
    HardwareDescriptorRing *ringInitialize;

    /**
     * Inherited methods from TransmitRing.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void initialize() final;
    void sendPacket(uint64_t *physicalAddress, uint16_t length) final;
};


#endif //HHUOS_TRANSMITRINGSIMPLE_H
