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
 * 317453006EN.PDF Revision 4.0. 2009
 */

#ifndef HHUOS_RECEIVERINGSIMPLET_H
#define HHUOS_RECEIVERINGSIMPLET_H


#include <cstdint>
#include "devices/network/e1000/general/Descriptors.h"
#include "devices/network/e1000/descriptorRing/HardwareDescriptorRing.h"

#include "ReceiveControl.h"
#include "descriptor/ReceiveDescriptor.h"
#include "ReceiveRing.h"
#include "kernel/events/network/ReceiveEvent.h"
/**
 * This class implements the ReceiveRing interface.
 *
 * Incoming packets will be send by the event bus to
 * a registered receiver.
 */
class ReceiveRingSimple final : public ReceiveRing {
public:
    /**
     * Initializes the attributes.
     * The event buffer will be initialized with 1024.
     * @param descriptors The descriptors contained in the receive descriptor block.
     * @param initialize The hardware descriptor ring initializer for receive.
     */
    explicit ReceiveRingSimple(Descriptors<ReceiveDescriptor *> *descriptors, HardwareDescriptorRing *initialize);
    ~ReceiveRingSimple() override;

private:
    /**
     * Descriptors of a coherent descriptor-block.
     */
    Descriptors<ReceiveDescriptor *> *descriptors;

    /**
     * Initialize the ring.
     */
    HardwareDescriptorRing *ringInitialize;

    /**
     * Publishes received packets.
     */
    EventBus *eventBus;

    /**
     * Stores incoming packets for publishing via
     * the event bus.
     */
    Util::RingBuffer<ReceiveEvent> eventBuffer;

    /**
     * Inherited methods from ReceiveRing.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void initialize() final;
    void receivePoll(Logger *log, uint8_t *mioBase) final;
};


#endif //HHUOS_RECEIVERINGSIMPLET_H
