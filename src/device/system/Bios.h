/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_DEVICE_BIOS_H
#define HHUOS_DEVICE_BIOS_H

#include <stdint.h>

#include "kernel/interrupt/InterruptDescriptorTable.h"
#include "kernel/process/Thread.h"

namespace Util {
namespace Async {
class Spinlock;
}  // namespace Async
}  // namespace Util

namespace Kernel {
class GlobalDescriptorTable;
}  // namespace Kernel

namespace Device {

/**
 * Provides static functions, that allow performing BIOS calls in Protected Mode
 */
class Bios {

public:

    enum RegisterHalf : uint8_t {
        LOWER,
        HIGHER
    };

    struct RealModeContext {
        uint16_t ds;
        uint16_t es;
        uint16_t fs;
        uint16_t gs;
        uint16_t flags;
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t esp;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;

        RealModeContext(const Kernel::Thread::Context &context);
        operator Kernel::Thread::Context();
    } __attribute__((packed));

    static uint16_t construct16BitRegister(uint8_t lowerValue, uint8_t higherValue);

    static uint8_t get8BitRegister(uint16_t value, RegisterHalf half);

    /**
     * Default-Constructor.
     * Deleted, as this class has only static members.
     */
    Bios() = delete;

    /**
     * Copy Constructor.
     */
    Bios(const Bios &other) = delete;

    /**
     * Assignment operator.
     */
    Bios &operator=(const Bios &other) = delete;

    /**
     * Destructor.
     */
    ~Bios() = default;

    static bool isAvailable();

    /**
     * Initializes segment for bios call.
     * Builds up a 16-bit code segment manually. The start address of this code segment is in the GDT for bios calls.
     */
    static void initialize();

    /**
     * Provides a bios call via software interrupt
     *
     * @param Interrupt number number of the bios call
     * @param context Parameter struct for the bios call
     */
    static Kernel::Thread::Context interrupt(int interruptNumber, const Kernel::Thread::Context &context);

    static Kernel::Thread::Context protectedModeCall(const Kernel::GlobalDescriptorTable &gdt, uint32_t entryPoint, const Kernel::Thread::Context &context);

    static const constexpr uint32_t MAX_USABLE_ADDRESS = 0xfffff;

private:

    static Kernel::InterruptDescriptorTable::Descriptor *biosIdtDescriptor;
    static Kernel::GlobalDescriptorTable biosGdt;
    static Util::Async::Spinlock lock;
};

}

#endif