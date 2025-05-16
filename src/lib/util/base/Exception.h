/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_EXCEPTION_H
#define HHUOS_EXCEPTION_H

#include <stdint.h>

namespace Util {

class Exception {

public:

    enum Error : uint32_t {
        // Hardware exceptions
        DIVIDE_BY_ZERO = 0x00,
        DEBUG = 0x01,
        NON_MASKABLE_INTERRUPT = 0x02,
        BREAKPOINT = 0x03,
        OVERFLOW = 0x04,
        BOUND_RANGE_EXCEEDED = 0x05,
        INVALID_OPCODE = 0x06,
        DEVICE_NOT_AVAILABLE = 0x07,
        DOUBLE_FAULT = 0x08,
        COPROCESSOR_SEGMENT_OVERRUN = 0x09,
        INVALID_TSS = 0x0A,
        SEGMENT_NOT_PRESENT = 0x0B,
        STACK_SEGMENT_FAULT = 0x0C,
        GENERAL_PROTECTION_FAULT = 0x0D,
        PAGE_FAULT = 0x0E,
        RESERVED_01 = 0x0F,
        X87_FLOATING_POINT_EXCEPTION = 0x10,
        ALIGNMENT_CHECK = 0x11,
        MACHINE_CHECK = 0x12,
        SIMD_FLOATING_POINT_EXCEPTION = 0x13,
        VIRTUALIZATION_EXCEPTION = 0x14,
        RESERVED_02 = 0x15,
        RESERVED_03 = 0x16,
        RESERVED_04 = 0x17,
        RESERVED_05 = 0x18,
        RESERVED_06 = 0x19,
        RESERVED_07 = 0x1A,
        RESERVED_08 = 0x1B,
        RESERVED_09 = 0x1C,
        RESERVED_10 = 0x1D,
        SECURITY_EXCEPTION = 0x1E,
        RESERVED_11 = 0x1F,

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
        UNSUPPORTED_OPERATION = 0xd3
    };

    [[noreturn]] static void throwException(Error error, const char *message = "");

    [[nodiscard]] static const char* getExceptionName(Error error);

private:

    static const char *hardwareExceptions[];
    static const char *softwareExceptions[];

};

}

#endif