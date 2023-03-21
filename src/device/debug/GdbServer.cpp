/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <cstring>
#include "GdbServer.h"
#include "kernel/system/System.h"
#include "kernel/service/InterruptService.h"

namespace Device {

void GdbServer::start(SerialPort::ComPort portId) {
    port = new SerialPort(portId);
    asm volatile ("int $3");
}

void GdbServer::plugin() {
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    for (uint32_t i = 0; i < 32; i++) {
        if (i != Kernel::DEVICE_NOT_AVAILABLE && i != Kernel::PAGE_FAULT) {
            interruptService.assignInterrupt(static_cast<Kernel::InterruptVector>(i), *this);
        }
    }
}

void GdbServer::trigger(const Kernel::InterruptFrame &frame) {
    bool stepping = false;
    uint32_t signal;
    uint32_t address, length;
    uint8_t *ptr;
    signal = computeSignal(frame.interrupt);

    ptr = outBuffer;
    *ptr++ = 'T';
    *ptr++ = HEX_CHARACTERS[signal >> 4U];
    *ptr++ = HEX_CHARACTERS[signal & 0x0FU];

    *ptr++ = HEX_CHARACTERS[REG_ESP];
    *ptr++ = ':';
    ptr = memoryToHexString(reinterpret_cast<const uint8_t*>(&frame.esp), ptr, 4, false);
    *ptr++ = ';';

    *ptr++ = HEX_CHARACTERS[REG_EBP];
    *ptr++ = ':';
    ptr = memoryToHexString(reinterpret_cast<const uint8_t*>(&frame.ebp), ptr, 4, false);
    *ptr++ = ';';

    *ptr++ = HEX_CHARACTERS[REG_EIP];
    *ptr++ = ':';
    ptr = memoryToHexString(reinterpret_cast<const uint8_t*>(&frame.eip), ptr, 4, false);
    *ptr++ = ';';

    *ptr = '\0';

    sendPacket(outBuffer);

    while (true) {
        outBuffer[0] = 0;
        ptr = receivePacket();
        auto gdbRegisters = GdbRegisters::fromInterruptFrame(frame);
        auto &copyRegisters = reinterpret_cast<GdbRegisters&>(ptr);

        switch (*ptr++) {
            case GET_HALT_REASON:
                outBuffer[0] = 'S';
                outBuffer[1] = HEX_CHARACTERS[signal >> 4U];
                outBuffer[2] = HEX_CHARACTERS[signal & 0x0FU];
                outBuffer[3] = 0;
                break;
            case TOGGLE_DEBUG:
                break;
            case GET_REGISTERS:
                memoryToHexString(reinterpret_cast<const uint8_t*>(&gdbRegisters.eax), outBuffer, NUM_REGS_BYTES, false);
                break;
            case SET_REGISTERS:
                setFrameRegisters(const_cast<Kernel::InterruptFrame&>(frame), copyRegisters);
                outBuffer[0] = 'O';
                outBuffer[1] = 'K';
            case SET_REGISTER:
                uint32_t registerNumber;
                uint32_t registerValue;
                if (parseHexString(&ptr, &registerNumber) && *ptr++ == '=') {
                    hexStringToMemory(ptr, reinterpret_cast<uint8_t*>(&registerValue), 4, false);
                    setFrameRegister(const_cast<Kernel::InterruptFrame&>(frame), (uint8_t) registerNumber, registerValue);
                    outBuffer[0] = 'O';
                    outBuffer[1] = 'K';
                    break;
                }

                break;
            case READ_MEMORY_HEX:
                if (parseHexString(&ptr, &address)) {
                    if (*(ptr++) == ',') {
                        if (parseHexString(&ptr, &length)) {
                            ptr = nullptr;
                            memoryError = false;
                            memoryToHexString(reinterpret_cast<const uint8_t*>(address), outBuffer, length, true);

                            if (memoryError) {
                                outBuffer[0] = 'E';
                                outBuffer[1] = '0';
                                outBuffer[2] = '3';
                            }
                        }
                    }
                }

                if (ptr) {
                    outBuffer[0] = 'E';
                    outBuffer[1] = '0';
                    outBuffer[2] = '1';
                }

                break;
            case WRITE_MEMORY_HEX:
                if (parseHexString(&ptr, &address)) {
                    if (*(ptr++) == ',') {
                        if (parseHexString(&ptr, &length)) {
                            if (*(ptr++) == ':') {
                                memoryError = false;
                                hexStringToMemory(ptr, (uint8_t *) address, length, true);

                                if (memoryError) {
                                    outBuffer[0] = 'E';
                                    outBuffer[1] = '0';
                                    outBuffer[2] = '3';
                                } else {
                                    outBuffer[0] = 'O';
                                    outBuffer[1] = 'K';
                                }

                                ptr = nullptr;
                            }
                        }
                    }
                }

                if (ptr) {
                    outBuffer[0] = 'E';
                    outBuffer[1] = '0';
                    outBuffer[2] = '2';
                }

                break;
            case STEP:
                stepping = true;
            case CONTINUE:
                if (parseHexString(&ptr, &address)) {
                    const_cast<Kernel::InterruptFrame&>(frame).eip = address;
                }

                const_cast<Kernel::InterruptFrame&>(frame).eflags &= 0xfffffeff;

                if (stepping) {
                    const_cast<Kernel::InterruptFrame&>(frame).eflags |= 0x100;
                }

                return;
            default:
                break;
        }

        sendPacket(outBuffer);
    }
}

uint8_t* GdbServer::receivePacket() {
    uint8_t *buffer = &inBuffer[0];
    uint8_t checksum;
    uint8_t receivedChecksum;
    uint32_t count;
    uint8_t ch;

    while (true) {
        ch = port->readDirect();
        while (ch != '$') {
            ch = port->readDirect();
        }

        retry:
        checksum = 0;
        count = 0;
        while (count < BUFFER_SIZE - 1) {
            ch = port->readDirect();
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
            ch = port->readDirect();
            receivedChecksum = parseHexChar(ch) << 4U;

            ch = port->readDirect();
            receivedChecksum += parseHexChar(ch);

            if (checksum != receivedChecksum) {
                port->write('-');
            } else {
                port->write('+');

                if (buffer[2] == ':') {
                    port->write(buffer[0]);
                    port->write(buffer[1]);
                    return &buffer[3];
                }

                return &buffer[0];
            }
        }
    }
}

void GdbServer::sendPacket(const uint8_t *packet) {
    uint8_t checksum;
    uint32_t count;
    uint8_t ch;

    do {
        port->write('$');

        checksum = 0;
        count = 0;
        while ((ch = packet[count])) {
            port->write(ch);
            checksum += ch;
            count += 1;
        }

        port->write('#');
        port->write(HEX_CHARACTERS[checksum >> 4U]);
        port->write(HEX_CHARACTERS[checksum & 0x0FU]);
    } while (port->readDirect() != '+');
}

void GdbServer::setFrameRegister(Kernel::InterruptFrame &frame, uint8_t registerNumber, uint32_t value) {
    switch (registerNumber) {
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
            frame.cs = value;
            break;
        case 11:
            frame.ss = value;
            break;
        case 12:
            frame.ds = value;
            break;
        case 13:
            frame.es = value;
            break;
        case 14:
            frame.fs = value;
            break;
        case 15:
            frame.gs = value;
            break;
        default:
            break;
    }
}

void GdbServer::setFrameRegisters(Kernel::InterruptFrame &frame, GdbServer::GdbRegisters &gdbRegisters) {
    frame.eax = gdbRegisters.eax;
    frame.ebx = gdbRegisters.ebx;
    frame.ecx = gdbRegisters.ecx;
    frame.edx = gdbRegisters.edx;
    frame.esp = gdbRegisters.esp;
    frame.ebp = gdbRegisters.ebp;
    frame.esi = gdbRegisters.esi;
    frame.edi = gdbRegisters.edi;
    frame.eip = gdbRegisters.pc;
    frame.eflags = gdbRegisters.ps;
    frame.es = gdbRegisters.es;
    frame.ds = gdbRegisters.ds;
    frame.fs = gdbRegisters.fs;
    frame.gs = gdbRegisters.gs;
    frame.cs = gdbRegisters.cs;
    frame.ss = gdbRegisters.ss;
}

uint8_t *GdbServer::memoryToHexString(const uint8_t *memory, uint8_t *target, uint32_t count, bool mayFault) {
    if (memory == nullptr) {
        memoryError = true;
        return target;
    }

    uint32_t i;
    uint8_t ch;
    for (i = 0; i < count; i++) {
        ch = *memory++;
        if (mayFault && memoryError) {
            return target;
        }

        *target++ = HEX_CHARACTERS[ch >> 4U];
        *target++ = HEX_CHARACTERS[ch & 0x0FU];
    }

    *target = 0;

    return target;
}

uint8_t *GdbServer::hexStringToMemory(uint8_t *source, uint8_t *memory, uint32_t count, bool mayFault) {
    if (memory == nullptr) {
        memoryError = true;
        return source;
    }

    uint32_t i;
    uint8_t ch;
    for (i = 0; i < count; i++) {
        ch = parseHexChar(*source++) << 4U;
        ch = ch + parseHexChar(*source++);
        *memory++ = ch;

        if (mayFault && memoryError) {
            return source;
        }
    }

    return memory;
}

uint8_t GdbServer::parseHexChar(char ch) {
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

uint32_t GdbServer::parseHexString(uint8_t **sourceBuffer, uint32_t *targetValue) {
    uint32_t numChars = 0;
    uint8_t hexValue;
    *targetValue = 0;

    while (**sourceBuffer) {
        hexValue = parseHexChar(**sourceBuffer);
        if (hexValue < 16U) {
            *targetValue = (*targetValue << 4U) | hexValue;
            numChars++;
        } else {
            break;
        }

        (*sourceBuffer)++;
    }

    return numChars;
}

uint8_t GdbServer::computeSignal(uint32_t interrupt) {
    uint8_t signal;

    switch (interrupt) {
        case Kernel::InterruptVector::DIVISION_BY_ZERO:
            signal = 8;
            break;
        case Kernel::InterruptVector::DEBUG:
            signal = 5;
            break;
        case Kernel::InterruptVector::BREAKPOINT:
            signal = 5;
            break;
        case Kernel::InterruptVector::OVERFLOW:
            signal = 16;
            break;
        case Kernel::InterruptVector::BOUND_RANGE_EXCEEDED:
            signal = 16;
            break;
        case Kernel::InterruptVector::INVALID_OPCODE:
            signal = 4;
            break;
        case Kernel::InterruptVector::DEVICE_NOT_AVAILABLE:
            signal = 8;
            break;
        case Kernel::InterruptVector::DOUBLE_FAULT:
            signal = 7;
            break;
        case Kernel::InterruptVector::COPROCESSOR_SEGMENT_OVERRUN:
            signal = 11;
            break;
        case Kernel::InterruptVector::INVALID_TASK_STATE_SEGMENT:
            signal = 11;
            break;
        case Kernel::InterruptVector::SEGMENT_NOT_PRESENT:
            signal = 11;
            break;
        case Kernel::InterruptVector::STACK_SEGMENT_FAULT:
            signal = 11;
            break;
        case Kernel::InterruptVector::GENERAL_PROTECTION_FAULT:
            signal = 11;
            break;
        case Kernel::InterruptVector::PAGE_FAULT:
            signal = 11;
            break;
        case Kernel::InterruptVector::X87_FLOATING_POINT_EXCEPTION:
            signal = 7;
            break;
        default:
            signal = 7;
    }

    return signal;
}

GdbServer::GdbRegisters GdbServer::GdbRegisters::fromInterruptFrame(const Kernel::InterruptFrame &frame) {
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

}