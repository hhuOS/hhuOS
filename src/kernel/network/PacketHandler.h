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

#ifndef HHUOS_PACKETHANDLER_H
#define HHUOS_PACKETHANDLER_H

#include "kernel/event/Receiver.h"
#include "kernel/log/Logger.h"
#include "kernel/event/network/ReceiveEvent.h"

namespace Kernel {

/**
 * This class implements the Receiver interface.
 *
 *
 * In future, this class could be meant to unpack received packets and
 * dispatch them to other classes by sending other kinds of events rather
 * then a ReceiveEvent.
 */
    class PacketHandler final : public Receiver {
        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("PacketHandler");

        /**
         * Inherited method from Receiver.
         * This method is meant to be overridden and
         * implemented by this class.
         */
        void onEvent(const Event &event) override;
    };

}

#endif
