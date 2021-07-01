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

#include <kernel/event/network/EthernetReceiveEvent.h>
#include "PacketHandler.h"
#include "DebugPrintout.h"

namespace Kernel {
    //Private method!
    uint8_t PacketHandler::notifyEthernetModule(void *incomingPacket, uint16_t length) {
        auto *input = new NetworkByteBlock(length);
        if (input->appendStraightFrom(incomingPacket, length)) {
            log.error("Reading data into NetworkByteBlock failed, discarding");
            delete input;
            return 1;
        }
        if (input->bytesRemaining() != 0) {
            log.error("Incoming data could not be loaded completely, discarding");
            delete input;
            return 1;
        }
        //Reset index to zero to prepare reading headers and data
        if (input->resetIndex()) {
            log.error("Index reset for input byteBlock failed, discarding");
            delete input;
            return 1;
        }

#if PRINT_IN_ALL_BYTES == 1
        printf("\nIncoming Bytes (%d per line):\n%s\n", BYTES_PER_LINE,
               (char *) input->asString(0, input->getLength() - 1, BYTES_PER_LINE));
#endif
        //send input to EthernetModule via EventBus for further processing
        eventBus->publish(new EthernetReceiveEvent(input));

        //We need input in EthernetModule
        //-> no 'delete input' here
        return 0;
    }

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
                log.error("Incoming data was empty, ignoring");
                delete (uint8_t *) packet;
                return;
            }
            if (notifyEthernetModule(packet, length)) {
                log.error("Could not notify EthernetModule, see syslog for more details");
            }
            //Processing finally done, cleanup
            delete (uint8_t *) packet;
            return;
        }
    }
}