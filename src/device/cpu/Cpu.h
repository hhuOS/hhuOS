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

#ifndef __CPU_include__
#define __CPU_include__

#include <cstdint>

#include "lib/util/Exception.h"
#include "lib/util/data/Array.h"

namespace Device {

/**
 * CPU - Provides abstraction and functionality around the CPU. Interrupts
 * can be enabled and disabled here and exceptions be handled.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */
class Cpu {

public:

    enum Configuration0 {
        PROTECTED_MODE_ENABLE = 0x01,
        MONITOR_CO_PROCESSOR = 0x02,
        X87_FPU_EMULATION = 0x04,
        TASK_SWITCHED = 0x08,
        EXTENSION_TYPE = 0x10,
        NUMERIC_ERROR = 0x20,
        WRITE_PROTECT = 0x10000,
        ALIGNMENT_MASK = 0x40000,
        NOT_WRITE_THROUGH = 0x20000000,
        CACHE_DISABLE = 0x40000000,
        PAGING = 0x80000000
    };

    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Cpu() = delete;

    /**
     * Copy Constructor.
     */
    Cpu(const Cpu &other) = delete;

    /**
     * Assignment operator.
     */
    Cpu &operator=(const Cpu &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~Cpu() = delete;

    /**
     * Enable hardware interrupts on CPU.
     */
    static void enableInterrupts();

    /**
     * Disable hardware interrupts on CPU.
     */
    static void disableInterrupts();

    static Util::Data::Array<Configuration0> readCr0();

    /**
     * Stop the processor via hlt instruction.
     */
    [[noreturn]] static void halt();

    /**
     * Enumeration of all hardware exceptions
     */
    enum class Error : uint32_t {
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
    };

    /**
     * Throw an exception.
     *
     * @param error The exception number
     * @param message An error message, that will be shown on the bluescreen
     */
    [[noreturn]] static void throwException(Util::Exception::Error error, const char *message);

    static const char *getExceptionName(uint32_t exception);

private:

    // Pointers to lists with hardware (software) exceptions
    static const char *hardwareExceptions[];
    static const char *softwareExceptions[];

    /**
     * Keeps track of how often disableInterrupts() and enableInterrupts() have been called.
     * Interrupts stay disabled, as long as this number is greater than zero.
     */
    static int32_t cliCount;
};

}

#endif
