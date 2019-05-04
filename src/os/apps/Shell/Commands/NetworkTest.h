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

#ifndef HHUOS_NETWORKTEST_H
#define HHUOS_NETWORKTEST_H

#include <devices/network/NetworkDevice.h>
#include <kernel/events/network/ReceiveEvent.h>
#include "apps/Shell/Commands/Command.h"

/**
 * This class implements the abstract class Command
 *
 * It can be used from the shell via the command 'nettest'.
 */
class NetworkTest : public Command {

public:

    NetworkTest() = delete;
    NetworkTest(const NetworkTest &copy) = delete;
    ~NetworkTest() override = default;

    /**
     * Constructor.
     */
    explicit NetworkTest(Shell &shell);

    /**
     * Send as much packets as possible in a given time and measure the throughput.
     *
     * @param physPacketAddress The physical address of the buffer, where the packets are stored
     * @param packetLength Size of the packets
     * @param driver The corresponding driver from whom the packets should be send
     * @param millis Duration of sending
     *
     * @return Data throughput in Mb/s.
     */
    void timeSendBenchmark(void *physPacketAddress, int packetLength, NetworkDevice &driver, uint32_t millis);

    /**
     * Send a given amount of packets and measure the throughput.
     *
     * @param physPacketAddress The physical address of the buffer, where the packets are stored
     * @param packetLength Size of the packets
     * @param driver The corresponding driver from whom the packets should be send
     * @param count Amount of packets to send
     *
     * @return Data throughput in Mb/s.
     */
    void packetSendBenchmark(void *physPacketAddress, int packetLength, NetworkDevice &driver, uint32_t count);

    /**
     * Overriding function from Command.
     */
    void execute(Util::Array<String> &args) override;

    /**
     * Overriding function from Command.
     */
    const String getHelpText() override;
};


#endif