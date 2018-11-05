/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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
 */

#ifndef HHUOS_ISA_H
#define HHUOS_ISA_H

#include <kernel/IOport.h>
#include <lib/lock/Spinlock.h>
#include <kernel/memory/manager/IsaDmaMemoryManager.h>

/**
 * Contains static functions, which can be used to communicate with the ISA DMA controller.
 *
 * Each buffer, that is used for ISA DMA must be allocated with Isa::allocDmaBuffer()
 * and freed with Isa::freeDmaBuffer().
 */
class Isa {

private:

    static IsaDmaMemoryManager dmaMemoryManager;

    static IOport startAddressRegisters[8];
    static IOport pageAddressRegisters[8];
    static IOport countRegisters[8];
    static IOport statusRegisters[2];
    static IOport commandRegisters[2];
    static IOport requestRegisters[2];
    static IOport singleChannelMaskRegisters[2];
    static IOport modeRegisters[2];
    static IOport flipFlopResetRegisters[2];
    static IOport intermediateRegisters[2];
    static IOport masterResetRegisters[2];
    static IOport maskResetRegisters[2];
    static IOport multiChannelMaskRegisters[2];

public:

    enum TransferMode : uint8_t {
        TRANSFER_MODE_SELF_TEST = 0x00,
        TRANSFER_MODE_WRITE = 0x04,
        TRANSFER_MODE_READ = 0x08
    };

    enum DmaMode : uint8_t {
        DMA_MODE_TRANSFER_ON_DEMAND = 0x00,
        DMA_MODE_SINGLE_TRANSFER = 0x40,
        DMA_MODE_BLOCK_TRANSFER = 0x80,
        DMA_MODE_CASCADE = 0xc0
    };

    /**
     * Allocate a 64KB buffer, that can be used for ISA DMA transfers.
     */
    static void *allocDmaBuffer();

    /**
     * Free a buffer, that has been allocated by Isa::allocDmaBuffer().
     *
     * @param ptr The buffer
     */
    static void freeDmaBuffer(void *ptr);

    /**
     * Select a DMA channel for further operations.
     *
     * @param channel The channel number (0-7)
     */
    static void selectChannel(uint8_t channel);

    /**
     * Deselect a DMA channel.
     *
     * @param channel The channel number (0-7)
     */
    static void deselectChannel(uint8_t channel);

    /**
     * Set the address for a DMA transfer.
     *
     * @param channel The channel number (0-7)
     * @param address The address (must point to a buffer, that has been allocated by Isa::allocDmaBuffer()).
     */
    static void setAddress(uint8_t channel, uint32_t address);

    /**
     * Set the amount of bytes to transfer.
     *
     * @param channel The channel number (0-7)
     * @param count The amount of bytes
     */
    static void setCount(uint8_t channel, uint16_t count);

    /**
     * Set the controller's operating mode.
     *
     * @param channel The channel number (0-7)
     * @param transferMode The transfer mode to use (read/write)
     * @param autoReset true, if the address and counters shall be reset after a DMA transfer
     * @param reverseMemoryOrder true, if the memory order shall be reversed (High addresses to low addresses)
     * @param dmaMode The DMA to use
     */
    static void setMode(uint8_t channel, TransferMode transferMode, bool autoReset, bool reverseMemoryOrder,
                        DmaMode dmaMode);

    /**
     * Reset the controller's mask register (all bits are set to OFF).
     *
     * @param channel The channel number (0-7)
     */
    static void resetMask(uint8_t channel);

    /**
     * Reset the controller's flip-flop, clear the status register and set all bits in the mask register to ON.
     *
     * @param channel The channel number (0-7)
     */
    static void resetAll(uint8_t channel);

private:

    /**
     * Reset the controller's flip-flop.
     *
     * @param channel The channel number (0-7)
     */
    static void resetFlipFlop(uint8_t channel);
};

#endif
