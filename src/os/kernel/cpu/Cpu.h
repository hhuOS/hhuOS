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


/**
 * CPU - Provides abstraction and functionality around the CPU. Interrupts
 * can be enabled and disabled here and exceptions be handled.
 *
 * @author Michael Schoettner, Filip Krakowski, Christian Gesse, Fabian Ruhland, Burak Akguel
 * @date HHU, 2018
 */
class Cpu {

public:
    
    Cpu() = delete;

    Cpu(const Cpu &other) = delete;

    Cpu &operator=(const Cpu &other) = delete;

    ~Cpu() = default;

    /**
     * Checks if interrupt flag is set in EFLAGS.
     *
     * @return Value of the interrupt flag in EFLAGS
     */
    static bool isInterrupted();
    
    /**
     * Enables hardware interrupts on CPU.
     */
    static void enableInterrupts();
    
    /**
     * Disables hardware interrupts on CPU.
     */
    static void disableInterrupts();
    
    /**
     * Stop CPU unitl next interrupt.
     */
    static void idle ();
    
    /**
     * Stop the processor via hlt instruction.
     */
    static void halt () __attribute__((noreturn));
 
    /**
     * Reads the time stamp counter.
     *
     * @return The current time stamp counter value.
     */
    static unsigned long long int rdtsc();

    // enumeration of hardware exceptions
    enum class Error : uint32_t {
        DIVIDE_BY_ZERO                      = 0x00,
        DEBUG                               = 0x01,
        NON_MASKABLE_INTERRUPT              = 0x02,
        BREAKPOINT                          = 0x03,
        OVERFLOW                            = 0x04,
        BOUND_RANGE_EXCEEDED                = 0x05,
        INVALID_OPCODE                      = 0x06,
        DEVICE_NOT_AVAILABLE                = 0x07,
        DOUBLE_FAULT                        = 0x08,
        COPROCESSOR_SEGMENT_OVERRUN         = 0x09,
        INVALID_TSS                         = 0x0A,
        SEGMENT_NOT_PRESENT                 = 0x0B,
        STACK_SEGMENT_FAULT                 = 0x0C,
        GENERAL_PROTECTION_FAULT            = 0x0D,
        PAGE_FAULT                          = 0x0E,
        RESERVED_01                         = 0x0F,
        X87_FLOATING_POINT_EXCEPTION        = 0x10,
        ALIGNMENT_CHECK                     = 0x11,
        MACHINE_CHECK                       = 0x12,
        SIMD_FLOATING_POINT_EXCEPTION       = 0x13,
        VIRTUALIZATION_EXCEPTION            = 0x14,
        RESERVED_02                         = 0x15,
        RESERVED_03                         = 0x16,
        RESERVED_04                         = 0x17,
        RESERVED_05                         = 0x18,
        RESERVED_06                         = 0x19,
        RESERVED_07                         = 0x1A,
        RESERVED_08                         = 0x1B,
        RESERVED_09                         = 0x1C,
        RESERVED_10                         = 0x1D,
        SECURITY_EXCEPTION                  = 0x1E,
        RESERVED_11                         = 0x1F,

    };
    // enumeration of software exceptions
    enum class Exception : uint32_t {
        NULLPOINTER                         = 0xC8,
        OUTOFBOUNDS                         = 0xC9,
        INVALID_ARGUMENT                    = 0xCA,
        KEY_NOT_FOUND                       = 0xCB,
        ILLEGAL_STATE                       = 0xCC,
        OUT_OF_MEMORY                       = 0XCD,
        OUT_OF_PHYS_MEMORY                  = 0xCE,
        OUT_OF_PAGE_MEMORY                  = 0xCF,
		ILLEGEAL_PAGE_ACCESS				= 0xD0,
		UNKNOWN_TYPE                        = 0xD1,
		PAGING_ERROR                        = 0xD2,
		UNSUPPORTED_OPERATION               = 0xD3
    };

    static void softInterrupt(uint32_t function);

    /**
     * Throws an exception.
     */
    static void throwException(Exception exception, const char *message = "");

    /**
     * Returns the name of the Exception from the number.
     *
     * @return String with name of exception
     */
    static const char *getExceptionName(uint32_t exception);

    /**
     * Returns the name of the Exception from the enumeration.
     *
     * @return String with name of exception
     */
    static const char *getExceptionName(Error exception);

    // Pointers to lists with hardware(software exceptions
    static const char *hardwareExceptions[];
    static const char *softwareExceptions[];

    // start number for software exceptions
    static const uint32_t SOFTWARE_EXCEPTIONS_START = 200;

private:

    static int32_t cliCount;
};

#endif
