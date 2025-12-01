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

#ifndef HHUOS_LIB_UTIL_PANIC_H
#define HHUOS_LIB_UTIL_PANIC_H

#include <stdint.h>

/// Provides functions to indicate critical errors.
namespace Util::Panic {

    /// Contains the error codes that can be used to indicate a critical error.
    /// The first 32 error codes are reserved for CPU exceptions.
    enum Error : uint32_t {
        // CPU exceptions
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
        INVALID_TSS = 0x0a,
        SEGMENT_NOT_PRESENT = 0x0b,
        STACK_SEGMENT_FAULT = 0x0c,
        GENERAL_PROTECTION_FAULT = 0x0d,
        PAGE_FAULT = 0x0e,
        RESERVED_01 = 0x0f,
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
        RESERVED_07 = 0x1a,
        RESERVED_08 = 0x1b,
        RESERVED_09 = 0x1c,
        RESERVED_10 = 0x1d,
        SECURITY_EXCEPTION = 0x1e,
        RESERVED_11 = 0x1f,

        // Software defined errors
        NULL_POINTER,
        OUT_OF_BOUNDS,
        INVALID_ARGUMENT,
        KEY_NOT_FOUND,
        ILLEGAL_STATE,
        OUT_OF_MEMORY,
        PAGING_ERROR,
        STACK_OVERFLOW,
        UNSUPPORTED_OPERATION
    };

    /// This function can be called to indicate a critical (unrecoverable) error.
    /// This will terminate the program and print the error message to the standard error output.
    ///
    /// ### Example
    /// ```c++
    /// void someFunction(int positiveNumber) {
    ///     if (positiveNumber < 0) {
    ///         Util::Panic::fire(Util::Panic::INVALID_ARGUMENT, "Negative number given!");
    ///     }
    /// }
    /// ```
    [[noreturn]] void fire(Error error, const char *message = "");

    /// Return the string representation of the given error.
    const char* getErrorAsString(Error error);

}

#endif