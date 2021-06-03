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
#include <kernel/service/EventBus.h>
#include <kernel/event/network/EthernetReceiveEvent.h>
#include "PacketHandler.h"

namespace Kernel {

    PacketHandler::PacketHandler(NetworkEventBus *eventBus) : eventBus(eventBus) {}

    void PacketHandler::onEvent(const Event &event) {
        if ((event.getType() == ReceiveEvent::TYPE)) {
            log.info("Incoming packet received");
            auto &receiveEvent = (ReceiveEvent &) event;
            if (receiveEvent.getLength() == 0) {
                log.info("Incoming data was empty, return");
                return;
            }
            if (receiveEvent.getPacket() == nullptr) {
                log.error("Incoming data was null, return");
                return;
            }
            auto *byteBlock = new NetworkByteBlock(receiveEvent.getLength());
            byteBlock->appendBytesStraight(receiveEvent.getPacket(), receiveEvent.getLength());
            receiveEvent.dropPacket();

            eventBus->publish(
                    new Kernel::EthernetReceiveEvent(new EthernetFrame(byteBlock))
            );
            return;
        }
    }

}