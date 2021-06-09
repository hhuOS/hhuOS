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
#include <kernel/event/network/EthernetReceiveEvent.h>
#include "PacketHandler.h"

namespace Kernel {

    PacketHandler::PacketHandler(NetworkEventBus *eventBus) : eventBus(eventBus) {}

    void PacketHandler::onEvent(const Event &event) {
        if ((event.getType() == ReceiveEvent::TYPE)) {
            auto length = ((ReceiveEvent &) event).getLength();
            auto *packet = ((ReceiveEvent &) event).getPacket();

            if (packet == nullptr) {
                log.error("Incoming data was null, ignoring");
                return;
            }
            if (length == 0) {
                log.info("Incoming data was empty, ignoring");
                delete (uint8_t *) packet;
                return;
            }

            auto *input = new NetworkByteBlock(length);
            if (input->append(packet, length)) {
                log.error("Reading of data into NetworkByteBlock failed, discarding");
                delete (uint8_t *) packet;
                delete input;
                return;
            }
            //Incoming packet is in NetworkByteBlock now
            //-> packet can be deleted
            delete (uint8_t *) packet;

            if (!input->isCompletelyFilled()) {
                log.error("Incoming data could not be loaded completely, discarding input");
                delete input;
                return;
            }
            //Reset index to zero to prepare reading headers and data
            if (input->decreaseIndex(input->getLength())) {
                log.error("Index reset for input byteBlock failed, discarding");
                delete input;
                return;
            }

            auto *inFrame = new EthernetFrame();
            if (inFrame->parseHeader(input)) {
                log.error("Parsing incoming packet as EthernetFrame failed, discarding");
                delete input;
                delete inFrame;
                return;
            }

            eventBus->publish(new EthernetReceiveEvent(inFrame, input));

            //inFrame and input will be deleted in EthernetModule after procesing
            //-> no delete here
            return;
        }
    }
}