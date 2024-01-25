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

#ifndef HHUOS_INTERRUPTVECTOR_H
#define HHUOS_INTERRUPTVECTOR_H

namespace Kernel {

enum InterruptVector : uint8_t {
    // Hardware exceptions
    DIVISION_BY_ZERO = 0,
    DEBUG = 1,
    NON_MASKABLE_INTERRUPT = 2,
    BREAKPOINT = 3,
    OVERFLOW = 4,
    BOUND_RANGE_EXCEEDED = 5,
    INVALID_OPCODE = 6,
    DEVICE_NOT_AVAILABLE = 7,
    DOUBLE_FAULT = 8,
    COPROCESSOR_SEGMENT_OVERRUN = 9,
    INVALID_TASK_STATE_SEGMENT = 10,
    SEGMENT_NOT_PRESENT = 11,
    STACK_SEGMENT_FAULT = 12,
    GENERAL_PROTECTION_FAULT = 13,
    PAGE_FAULT = 14,
    X87_FLOATING_POINT_EXCEPTION = 16,
    ALIGNMENT_CHECK = 17,
    MACHINE_CHECK = 18,
    SIMD_FLOATING_POINT_EXCEPTION = 19,
    VIRTUALIZATION_EXCEPTION = 20,
    CONTROL_PROTECTION_EXCEPTION = 21,
    HYPERVISOR_INJECTION_EXCEPTION = 28,
    VMM_COMMUNICATION_EXCEPTION = 29,
    SECURITY_EXCEPTION = 30,

    // PC/AT compatible interrupts
    PIT = 32,
    KEYBOARD = 33,
    COM2 = 35,
    COM1 = 36,
    LPT2 = 37,
    FLOPPY = 38,
    LPT1 = 39,
    RTC = 40,
    FREE1 = 41,
    FREE2 = 42,
    UHCI = 43,
    MOUSE = 44,
    FPU = 45,
    PRIMARY_ATA = 46,
    SECONDARY_ATA = 47,
    // Possibly some other interrupts supported by IO APICs

    SYSTEM_CALL = 0x86,

    // Software exceptions
    NULL_POINTER = 0xc8,
    OUT_OF_BOUNDS = 0xc9,
    INVALID_ARGUMENT = 0xca,
    KEY_NOT_FOUND = 0xcb,
    ILLEGAL_STATE = 0xcc,
    OUT_OF_MEMORY = 0Xcd,
    OUT_OF_PHYSICAL_MEMORY = 0xce,
    OUT_OF_PAGING_MEMORY = 0xcf,
    ILLEGAL_PAGE_ACCESS = 0xd0,
    CLASS_NOT_FOUND = 0xd1,
    PAGING_ERROR = 0xd2,
    UNSUPPORTED_OPERATION = 0xd3,

    // Local APIC interrupts (247 - 254)
    CMCI = 0xf8,
    APICTIMER = 0xf9,
    THERMAL = 0xfa,
    PERFORMANCE = 0xfb,
    LINT0 = 0xfc,
    LINT1 = 0xfd,
    ERROR = 0xfe,

    SPURIOUS = 0xff
};

}

#endif
