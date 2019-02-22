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

#include <devices/network/e1000/E1000.h>
#include <kernel/events/network/ReceiveEvent.h>
#include "apps/Shell/Commands/Command.h"

/**
 * This class implements the abstract classes Command
 * and Receiver.
 *
 * It can be used from the shell via the command 'nettest'.
 */
class NetworkTest : public Command, public Receiver {

public:
    /**
     * Total received packets since start of hhuOS.
     */
    uint16_t received = 0;

    /**
     * Counts all transmitted packets since start of hhuOS,
     * but those who are resent via the sendFromBuffer method.
     */
    uint32_t transmitted = 0;

    /**
     * Total of those packets who have been read
     * due to printRingBufferPacket method.
     */
    uint16_t read = 0;

    /**
     * Counts the amount of packets which are send
     * from the storedSendBuffer
     */
    uint32_t storedSend = 0;

    /**
     * A buffer where all incoming events are stored. One can
     * read from it via printRingBufferPacket.
     * Will be initialized with 1024.
     */
    Util::RingBuffer<ReceiveEvent> eventBuffer;
    /**
     * Stores incoming packets like the eventBuffer.
     * Packets from this buffer can be pop-ed an
     * send.
     */
    Util::RingBuffer<ReceiveEvent> storedSendBuffer;

    /**
     * It is used to synchronize the eventBuffer and the storedSendBuffer.
     * Otherwise there would be a lost update problem with
     * the current implementation if both buffer are empty,
     * and packets are sent.
     */
    Spinlock lock;

    NetworkTest() = delete;
    NetworkTest(const NetworkTest &copy) = delete;
    ~NetworkTest() override = default;

    /**
     * Initializes the attributes. The attributes 'eventBuffer' and
     * 'storedSendBuffer' will be initialized with 1024.
     * @param shell The shell, that executes this command.
     */
    explicit NetworkTest(Shell &shell);

    /**
     * Sends as much packets of a given size as possible for a given
     * time.
     * @param size Size of the packets.
     * @param driver The corresponding driver from whom the packets should be send.
     * @param phyAddress The physical address of the buffer, where the packets are stored.
     * @param millis Duration of sending.
     * @return Data throughput in Mb/s.
     */
    int measurePackets(int size, E1000 *driver, uint64_t *phyAddress, int millis);

    /**
     * Pops a packet from the eventBuffer and displays it on the shell.
     */
    void printRingBufferPacket();

    /**
     * Pops a given amount of packets from the storedSendBuffer and
     * sends them.
     * @param packets Amount of packets to send.
     * @param driver The corresponding drivver from whom the packets should be send.
     */
    void sendFromBuffer(int packets, E1000 *driver);

    /**
     * Displays the current network information according to ReceiveEvents.
     */
    void printInfo();

    /**
     * Inherited methods from Command.
     * This methods are meant to be overridden and
     * implemented by this class.
     */

    void execute(Util::Array<String> &args) override;
    const String getHelpText() override;

    /**
     * Inherited method from Receiver.
     * This method is meant to be overridden and
     * implemented by this class.
     */

    void onEvent(const Event &event) override;
};


#endif //HHUOS_NETWORKTEST_H