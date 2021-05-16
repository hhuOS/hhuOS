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

#include <kernel/network/ethernet/EthernetFrame.h>
#include <kernel/core/System.h>
#include <kernel/service/EventBus.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include "PacketHandler.h"

namespace Kernel {

    void PacketHandler::onEvent(const Event &event) {
        if ((event.getType() == ReceiveEvent::TYPE)) {
            auto &receiveEvent = (ReceiveEvent &) event;
            auto *inFrame = new EthernetFrame(receiveEvent.getPacket(), receiveEvent.getLength());
            auto *eventBus = Kernel::System::getService<Kernel::EventBus>();
            eventBus->publish(
                    Util::SmartPointer<Kernel::Event>(
                            new Kernel::EthernetReceiveEvent(inFrame)
                    )
            );
            return;
        }
    }

}