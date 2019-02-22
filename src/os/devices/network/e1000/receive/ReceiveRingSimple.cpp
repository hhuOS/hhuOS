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

#include <kernel/memory/SystemManagement.h>
#include <kernel/Kernel.h>
#include <kernel/services/EventBus.h>
#include "ReceiveRingSimple.h"


ReceiveRingSimple::ReceiveRingSimple(Descriptors<ReceiveDescriptor *> *descriptors,
                     HardwareDescriptorRing *initialize) : descriptors(descriptors), ringInitialize(initialize), eventBuffer(1024)
{
    eventBus = Kernel::getService<EventBus>();
}

ReceiveRingSimple::~ReceiveRingSimple() = default;

void ReceiveRingSimple::initialize() {
    ringInitialize->initialize();
}

void ReceiveRingSimple::receivePoll(Logger *log, uint8_t *mioBase) {
    auto descriptor = descriptors->current();

    //todo mark with taken from
    while( descriptor->done() ) {
        // raw packet and packet length (excluding CRC)/
        uint16_t pktlen = descriptor->readLength();
        volatile uint8_t *pkt = descriptor->getPacketAddress();

        bool drop = false;

        if( pktlen < 64 )
        {
            //log->info("Dropped short packet of %u bytes", pktlen);
            drop = true;
        }

        if( !(descriptor->isEndOfPacked()) )
        {
            //log->info("Dropped packet (EOP not set) (len=%u)", pktlen);
            drop = true;
        }

        if( descriptor->hasErrors() )
        {
            //log->info("There is an Error at received packet");
            drop = true;
        }

        if( !drop )
        {
            //log->info("Incoming Packet with length: %d", pktlen);

            eventBuffer.push(ReceiveEvent(pkt, pktlen));
            ReceiveEvent &event = eventBuffer.pop();
            eventBus->publish(event);
        }

        descriptor->clearStatus();

        descriptors->setNext();
        ringInitialize->updateTail();
    }
}

