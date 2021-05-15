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

#ifndef HHUOS_RECEIVEEVENT_H
#define HHUOS_RECEIVEEVENT_H

#include <cstdint>
#include "kernel/event/Event.h"

namespace Kernel {

/**
 * This class implements the abstract class
 * Event.
 *
 * Classes get register themself as Receiver on this
 * event to get packages from the ReceiveRing of an
 * E1000 driver.
 *
 * It provides all relevant information of the packet:
 * Its length and the whole packet itself.
 */
    class ReceiveEvent : public Event {

    public:
        ReceiveEvent() = default;

        explicit ReceiveEvent(void *packet, uint16_t length);

        ReceiveEvent(const ReceiveEvent &other);

        ~ReceiveEvent();

        String getType() const override;

        /**
         * @return A pointer to the packet that will be dispatched.
         */
        void *getPacket();

        /**
         * @return The length of the packet.
         */
        uint16_t getLength();

        static const constexpr char *TYPE = "ReceiveEvent";

    private:
        /**
         * Stores a pointer to the packet.
         */
        void *packet = nullptr;

        /**
         * Stores the length of the packet.
         */
        uint16_t length = 0;
    };

}

#endif