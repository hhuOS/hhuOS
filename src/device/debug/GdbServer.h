/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
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

#ifndef HHUOS_GDBSERVER_H
#define HHUOS_GDBSERVER_H

#include <cstdint>

#include "device/port/serial/SerialPort.h"
#include "kernel/interrupt/InterruptHandler.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

class GdbServer : Kernel::InterruptHandler {

public:
    /**
     * Contains all registers of a x86-CPU
     */
    struct GdbRegisters {
        uint32_t eax;
        uint32_t ecx;
        uint32_t edx;
        uint32_t ebx;
        uint32_t esp;
        uint32_t ebp;
        uint32_t esi;
        uint32_t edi;
        uint32_t pc;
        uint32_t ps;
        uint32_t cs;
        uint32_t ss;
        uint32_t ds;
        uint32_t es;
        uint32_t fs;
        uint32_t gs;

        static GdbRegisters fromInterruptFrame(const Kernel::InterruptFrame &frame);
    } __attribute__((packed));

    /**
     * Default Constructor.
     */
    GdbServer() = default;

    /**
     * Copy Constructor.
     */
    GdbServer(const GdbServer &other) = delete;

    /**
     * Assignment operator.
     */
    GdbServer &operator=(const GdbServer &other) = delete;

    /**
     * Destructor.
     */
    ~GdbServer() override = default;

    void start(SerialPort::ComPort portId);

    void plugin() override;

    void trigger(const Kernel::InterruptFrame &frame) override;

private:

    uint8_t* receivePacket();

    void sendPacket(const uint8_t *packet);

    uint8_t* memoryToHexString(const uint8_t *memory, uint8_t *target, uint32_t count, bool mayFault);

    uint8_t* hexStringToMemory(uint8_t *source, uint8_t *memory, uint32_t count, bool mayFault);

    static void setFrameRegister(Kernel::InterruptFrame &frame, uint8_t registerNumber, uint32_t value);

    static void setFrameRegisters(Kernel::InterruptFrame &frame, GdbRegisters &gdbRegisters);

    static uint8_t parseHexChar(char ch);

    static uint32_t parseHexString(uint8_t **sourceBuffer, uint32_t *targetValue);

    static uint8_t computeSignal(uint32_t interrupt);

    static const constexpr uint32_t BUFFER_SIZE = 400;
    static const constexpr uint8_t REG_ESP = 4;
    static const constexpr uint8_t REG_EBP = 5;
    static const constexpr uint8_t REG_EIP = 8;
    static const constexpr uint8_t NUM_REGS = 16;
    static const constexpr uint8_t NUM_REGS_BYTES = 16 * 4;

    SerialPort *port = nullptr;

    bool memoryError = false;
    uint8_t inBuffer[BUFFER_SIZE]{};
    uint8_t outBuffer[BUFFER_SIZE]{};

    static const constexpr char WRITE_MEMORY_BIN = 'X';
    static const constexpr char WRITE_MEMORY_HEX = 'M';
    static const constexpr char READ_MEMORY_HEX = 'm';
    static const constexpr char CONTINUE = 'c';
    static const constexpr char STEP = 's';
    static const constexpr char SET_THREAD = 'H';
    static const constexpr char TOGGLE_DEBUG = 'd';
    static const constexpr char GET_REGISTERS = 'g';
    static const constexpr char SET_REGISTERS = 'G';
    static const constexpr char SET_REGISTER = 'P';
    static const constexpr char GET_HALT_REASON = '?';

    static const constexpr char *HEX_CHARACTERS = "0123456789abcdef";
};

}

#endif
