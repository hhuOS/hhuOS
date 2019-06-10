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

#ifndef HHUOS_RECEIVEDESCRIPTOR_H
#define HHUOS_RECEIVEDESCRIPTOR_H

#include <cstdint>
#include "kernel/log/Logger.h"

/**
 * This is an interface for handling the receive descriptor.
 *
 * "A receive descriptor is a data structure that contains the receive data buffer address and fields for
 *  hardware to store packet information. [...]

 *  Note:
 *  Upon receipt of a packet for Ethernet controllers, hardware stores the packet data into the indicated
 *  buffer and writes the length, Packet Checksum, status, errors, and status fields.",
 *  [quote 3.2.3 Receive Descriptor Format]
 *
 *
 *      63          48 47    40 39    32 31                  16 15     0
 *     *--------------*--------*--------*----------------------*--------*
 * 0x0 |                     Buffer Address [63:0]                      |
 *     *--------------*--------*--------*----------------------*--------*
 * 0x8 | Special/Rsv. | Errors | Status | Packet Checksum/Rsv. | Length |
 *     *--------------*--------*--------*----------------------*--------*
 *
 * Descriptions of the bits are taken from the manual.
 * [see Table 3-1. Receive Descriptor (RDESC) Layout]
 */
class ReceiveDescriptor {
public:
    virtual ~ReceiveDescriptor() = default;

    ReceiveDescriptor(ReceiveDescriptor const &) = delete;
    ReceiveDescriptor &operator=(ReceiveDescriptor const &) = delete;

    /**
     * Writes the host receive-buffer address.
     * @param address Physical address of the buffer where the receive
     * package will be stored.
     */
    virtual void writeAddress(uint64_t address) = 0;

    /**
     * Retrieves the virtual address of the buffer, where
     * a received packet is stored. Is called, after
     * hardware is done with the descriptor.
     * @return The virtual address of the stored packet.
     */
    virtual volatile uint8_t * getPacketAddress() = 0;

    /**
     * Retrieves the length of the packet stored in the buffer
     * indicated in this descriptor.
     * @return The length of the stored packet.
     */
    virtual uint16_t readLength() = 0;

    /**
     * Indicates, whether the packet described by this descriptor
     * is the last of the packet or not.
     * @return True, if this descriptor is the last for it's stored packet.
     */
    virtual bool isEndOfPacked() = 0;

    /**
     * This is useful in cases where software checks the
     * descriptor-ring for descriptors finished by hardware.
     */
    virtual void clearStatus() = 0;

    /**
     * Checks if there have been any errors due packet reception.
     * @return True, if errors occurred, otherwise false.
     */
    virtual bool hasErrors() = 0;

    /**
     * Checks depending on updates of the status by the hardware
     * if this descriptor had been progressed already.
     * @return True, if this descriptor had been progressed, otherwise false.
     */
    virtual bool done() = 0;

    /**
     * This methods are not used for Intel 82541IP. For usage
     * please refer to the manual.
     */

    virtual uint16_t readPacketChecksum() = 0;
    virtual uint16_t readSpecial() = 0;

protected:
    ReceiveDescriptor() = default;
};


#endif //HHUOS_RECEIVEDESCRIPTOR_H
