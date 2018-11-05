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

#ifndef __GdbServer_include__
#define __GdbServer_include__

#define BREAKPOINT() asm("   int $3");

#include <kernel/threads/ThreadState.h>
#include "devices/ports/Serial.h"

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

    static GdbRegisters fromInterruptFrame(InterruptFrame &frame);
} __attribute__((packed));

class GdbServer {

public:

    GdbServer() = delete;

    GdbServer(const GdbServer &other) = delete;

    GdbServer &operator=(const GdbServer &other) = delete;

    virtual ~GdbServer();

    static void handleInterrupt(InterruptFrame &frame);

    static uint8_t computeSignal(uint32_t interrupt);

    static void initialize();

    static inline void synchronize() __attribute__((always_inline)) {
        BREAKPOINT();
    }

    static bool isInitialized();

    static bool memError;

private:

    static uint8_t* getPacket();

    static void putPacket(const uint8_t *packet);

    static void setFrameRegister(InterruptFrame &frame, uint8_t regNumber, uint32_t value);

    static void setFrameRegisters(InterruptFrame &frame, GdbRegisters &gdbRegs);

    static const uint32_t BUFMAX = 400;

    static const uint8_t REG_ESP = 4;

    static const uint8_t REG_EBP = 5;

    static const uint8_t REG_EIP = 8;

    static const uint8_t NUM_REGS = 16;

    static const uint8_t NUM_REGS_BYTES = 16 * 4;

    static uint8_t inBuffer[];

    static uint8_t outBuffer[];

    static Serial* serial;

    static const char WRITE_MEMORY_BIN = 'X';

    static const char WRITE_MEMORY_HEX = 'M';

    static const char READ_MEMORY_HEX = 'm';

    static const char CONTINUE = 'c';

    static const char STEP = 's';

    static const char SET_THREAD = 'H';

    static const char TOGGLE_DEBUG = 'd';

    static const char GET_REGISTERS = 'g';

    static const char SET_REGISTERS = 'G';

    static const char SET_REGISTER = 'P';

    static const char GET_HALT_REASON = '?';
};


#endif //HHUOS_GDBSERVER_H
