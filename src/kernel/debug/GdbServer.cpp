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

#include "GdbServer.h"

static const uint8_t *hexCharacters = (uint8_t*) "0123456789abcdef";

Port* GdbServer::port = nullptr;

uint8_t GdbServer::inBuffer[BUFMAX];

uint8_t GdbServer::outBuffer[BUFMAX];

bool GdbServer::memError = false;

static bool initialized = false;

GdbServer::~GdbServer() = default;

uint8_t hex(uint8_t ch) {

    if ((ch >= 'a') && (ch <= 'f')) {

        return (ch - 'a' + (uint8_t) 10);
    }

    if ((ch >= '0') && (ch <= '9')) {

        return (ch - '0');
    }

    if ((ch >= 'A') && (ch <= 'F')) {

        return (ch - 'A' + (uint8_t) 10);
    }

    return UINT8_MAX;
}

void setChar(uint8_t *address, uint8_t val) {
    
    *address = val;
}

uint8_t getChar(const uint8_t *addr) {

    return *addr;
}

uint32_t hexToInt(uint8_t **ptr, uint32_t *intValue) {

    uint32_t numChars = 0;

    uint8_t hexValue;

    *intValue = 0;

    while (**ptr) {

        hexValue = hex(**ptr);

        if (hexValue < 16U) {

            *intValue = (*intValue << 4U) | hexValue;

            numChars++;

        } else {

            break;
        }

        (*ptr)++;
    }

    return numChars;
}

uint8_t *mem2hex(uint8_t *mem, uint8_t *buf, uint32_t count, bool mayFault) {

    if (mem == nullptr) {

        GdbServer::memError = true;

        return buf;
    }

    uint32_t i;

    uint8_t ch;

    for (i = 0; i < count; i++) {

        ch = getChar(mem++);

        if (mayFault && GdbServer::memError) {

            return buf;
        }

        *buf++ = hexCharacters[ch >> 4U];

        *buf++ = hexCharacters[ch & 0x0FU];
    }

    *buf = 0;

    return (buf);
}


uint8_t *hex2mem(uint8_t *buf, uint8_t *mem, uint32_t count, bool mayFault) {

    if (mem == nullptr) {

        GdbServer::memError = true;

        return buf;
    }

    uint32_t i;

    uint8_t ch;

    for (i = 0; i < count; i++) {

        ch = hex(*buf++) << 4U;

        ch = ch + hex(*buf++);

        setChar(mem++, ch);

        if (mayFault && GdbServer::memError) {

            return buf;
        }
    }

    return (mem);
}

uint8_t* GdbServer::getPacket() {

    uint8_t *buffer = &inBuffer[0];

    uint8_t checksum;

    uint8_t xmitcsum;

    uint32_t count;

    uint8_t ch;

    while (1 > 0) {

        while ((ch = (uint8_t) port->readChar()) != '$');

        retry:

        checksum = 0;

        count = 0;

        while (count < BUFMAX - 1) {

            ch = (uint8_t) port->readChar();

            if (ch == '$') {

                goto retry;
            }

            if (ch == '#') {

                break;
            }

            checksum = checksum + ch;

            buffer[count] = ch;

            count = count + 1;
        }

        buffer[count] = 0;

        if (ch == '#') {

            ch = (uint8_t) port->readChar();

            xmitcsum = hex (ch) << 4U;

            ch = (uint8_t) port->readChar();

            xmitcsum += hex (ch);

            if (checksum != xmitcsum) {

                port->sendChar('-');

            } else {

                port->sendChar('+');

                if (buffer[2] == ':') {

                    port->sendChar(buffer[0]);

                    port->sendChar(buffer[1]);

                    return &buffer[3];
                }

                return &buffer[0];
            }
        }
    }
}

void GdbServer::putPacket(const uint8_t *buffer) {

    uint8_t checksum;

    uint32_t count;

    uint8_t ch;

    do {

        port->sendChar('$');

        checksum = 0;

        count = 0;

        while ((ch = buffer[count])) {

            port->sendChar(ch);

            checksum += ch;

            count += 1;
        }

        port->sendChar('#');

        port->sendChar(hexCharacters[checksum >> 4U]);

        port->sendChar(hexCharacters[checksum & 0x0FU]);

    }

    while ((uint8_t) port->readChar() != '+');
}

void GdbServer::setFrameRegisters(InterruptFrame &frame, GdbRegisters &gdbRegs) {

    frame.eax = gdbRegs.eax;

    frame.ebx = gdbRegs.ebx;

    frame.ecx = gdbRegs.ecx;

    frame.edx = gdbRegs.edx;

    frame.esp = gdbRegs.esp;

    frame.ebp = gdbRegs.ebp;

    frame.esi = gdbRegs.esi;

    frame.edi = gdbRegs.edi;

    frame.eip = gdbRegs.pc;

    frame.eflags = gdbRegs.ps;

    frame.es = (uint16_t) gdbRegs.es;

    frame.ds = (uint16_t) gdbRegs.ds;

    frame.fs = (uint16_t) gdbRegs.fs;

    frame.gs = (uint16_t) gdbRegs.gs;

    frame.cs = (uint16_t) gdbRegs.cs;

    frame.ss = (uint16_t) gdbRegs.ss;
}

void GdbServer::setFrameRegister(InterruptFrame &frame, uint8_t regNumber, uint32_t value) {

    switch (regNumber) {
        case 0:

            frame.eax = value;

            break;
        case 1:

            frame.ecx = value;

            break;
        case 2:

            frame.edx = value;

            break;
        case 3:

            frame.ebx = value;

            break;
        case 4:

            frame.esp = value;

            break;
        case 5:

            frame.ebp = value;

            break;
        case 6:

            frame.esi = value;

            break;
        case 7:

            frame.edi = value;

            break;
        case 8:

            frame.eip = value;

            break;
        case 9:

            frame.eflags = value;

            break;
        case 10:

            frame.cs = (uint16_t) value;

            break;
        case 11:

            frame.ss = (uint16_t) value;

            break;
        case 12:

            frame.ds = (uint16_t) value;

            break;
        case 13:

            frame.es = (uint16_t) value;

            break;
        case 14:

            frame.fs = (uint16_t) value;

            break;
        case 15:

            frame.gs = (uint16_t) value;

            break;
        default:
            break;
    }
}

void GdbServer::handleInterrupt(InterruptFrame &frame) {

    bool stepping = false;

    uint32_t sigval;

    uint32_t address, length;

    uint8_t *ptr;

    sigval = computeSignal(frame.interrupt);

    ptr = outBuffer;

    *ptr++ = 'T';

    *ptr++ = hexCharacters[sigval >> 4U];

    *ptr++ = hexCharacters[sigval & 0x0FU];

    *ptr++ = hexCharacters[REG_ESP];

    *ptr++ = ':';

    ptr = mem2hex((uint8_t *) &frame.esp, ptr, 4, false);

    *ptr++ = ';';

    *ptr++ = hexCharacters[REG_EBP];

    *ptr++ = ':';

    ptr = mem2hex((uint8_t *) &frame.ebp, ptr, 4, false);

    *ptr++ = ';';

    *ptr++ = hexCharacters[REG_EIP];

    *ptr++ = ':';

    ptr = mem2hex((uint8_t *) &frame.eip, ptr, 4, false);

    *ptr++ = ';';

    *ptr = '\0';

    putPacket(&outBuffer[0]);

    while (1 > 0) {

        outBuffer[0] = 0;

        ptr = getPacket();

        GdbRegisters gdbRegs = GdbRegisters::fromInterruptFrame(frame);

        auto &copyRegs = (GdbRegisters&) ptr;

        switch (*ptr++) {
            case GET_HALT_REASON:

                outBuffer[0] = 'S';

                outBuffer[1] = hexCharacters[sigval >> 4U];

                outBuffer[2] = hexCharacters[sigval & 0x0FU];

                outBuffer[3] = 0;

                break;
            case TOGGLE_DEBUG:

                /* toggle debug flag */

                break;
            case GET_REGISTERS:

                mem2hex((uint8_t *) &gdbRegs.eax, outBuffer, NUM_REGS_BYTES, false);

                break;
            case SET_REGISTERS:

                setFrameRegisters(frame, copyRegs);

                strcpy ((char*) outBuffer, "OK");

                break;
            case SET_REGISTER:

                uint32_t regno, regValue;

                if (hexToInt (&ptr, &regno) && *ptr++ == '=') {

                    hex2mem(ptr, (uint8_t*) &regValue, 4, false);

                    setFrameRegister(frame, (uint8_t) regno, regValue);

                    strcpy ((char*) outBuffer, "OK");

                    break;
                }

                break;
            case READ_MEMORY_HEX:

                if (hexToInt (&ptr, &address)) {

                    if (*(ptr++) == ',') {

                        if (hexToInt (&ptr, &length)) {

                            ptr = nullptr;

                            memError = false;

                            mem2hex((uint8_t *) address, outBuffer, length, true);

                            if (memError) {

                                strcpy((char*) outBuffer, "E03");
                            }
                        }
                    }
                }

                if (ptr) {

                    strcpy ((char*) outBuffer, "E01");
                }

                break;
            case WRITE_MEMORY_HEX:

                if (hexToInt (&ptr, &address)) {

                    if (*(ptr++) == ',') {

                        if (hexToInt (&ptr, &length)) {

                            if (*(ptr++) == ':') {

                                memError = false;

                                hex2mem(ptr, (uint8_t *) address, length, true);

                                if (memError) {

                                    strcpy((char*) outBuffer, "E03");

                                } else {

                                    strcpy ((char*) outBuffer, "OK");
                                }

                                ptr = nullptr;
                            }
                        }
                    }
                }

                if (ptr) {

                    strcpy ((char*) outBuffer, "E02");
                }

                break;
            case STEP:

                stepping = true;

            case CONTINUE:

                if (hexToInt (&ptr, &address)) {
                    frame.eip = address;
                }

                frame.eflags &= 0xfffffeff;

                if (stepping) {

                    frame.eflags |= 0x100;
                }

                return;
            default:
                break;
        }

        putPacket(outBuffer);
    }
}

uint8_t GdbServer::computeSignal(uint32_t interrupt) {
    uint8_t sigval;

    switch (interrupt) {

        case 0:

            sigval = 8;

            break;			/* divide by zero */
        case 1:

            sigval = 5;

            break;			/* debug exception */
        case 3:

            sigval = 5;

            break;			/* breakpoint */
        case 4:

            sigval = 16;

            break;			/* into instruction (overflow) */
        case 5:

            sigval = 16;

            break;			/* bound instruction */
        case 6:

            sigval = 4;

            break;			/* Invalid opcode */
        case 7:

            sigval = 8;

            break;			/* coprocessor not available */
        case 8:

            sigval = 7;

            break;			/* double fault */
        case 9:

            sigval = 11;

            break;			/* coprocessor segment overrun */
        case 10:

            sigval = 11;

            break;			/* Invalid TSS */
        case 11:

            sigval = 11;

            break;			/* Segment not present */
        case 12:

            sigval = 11;

            break;			/* stack exception */
        case 13:

            sigval = 11;

            break;			/* general protection */
        case 14:

            sigval = 11;

            break;			/* page fault */
        case 16:

            sigval = 7;

            break;			/* coprocessor error */
        default:

            sigval = 7;		/* "software generated" */
    }

    return sigval;
}

bool GdbServer::isInitialized() {

    return initialized;
}

void GdbServer::initialize(Port *port) {

    GdbServer::port = port;

    initialized = true;
}

GdbRegisters GdbRegisters::fromInterruptFrame(InterruptFrame &frame) {

    GdbRegisters registers{};

    registers.eax = frame.eax;

    registers.ebx = frame.ebx;

    registers.ecx = frame.ecx;

    registers.edx = frame.edx;

    registers.esp = frame.esp;

    registers.ebp = frame.ebp;

    registers.esi = frame.esi;

    registers.edi = frame.edi;

    registers.pc = frame.eip;

    registers.ps = frame.eflags;

    registers.cs = frame.cs;

    registers.ss = frame.ss;

    registers.gs = frame.gs;

    registers.fs = frame.fs;

    registers.es = frame.es;

    registers.ds = frame.ds;

    return registers;
}
