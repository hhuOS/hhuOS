/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_ASM_INTERFACE
#define HHUOS_ASM_INTERFACE

#include <cstdint>

namespace Kernel {
struct Context;
}  // namespace Kernel

struct CopyInformation {
    uint32_t sourceAddress;
    uint32_t targetAreaSize;
    uint32_t copiedBytes;
    bool success;
};

// Import asm variables
extern const uint32_t ___WRITE_PROTECTED_START__;
extern const uint32_t ___WRITE_PROTECTED_END__;
extern const uint32_t ___KERNEL_DATA_START__;
extern const uint32_t ___KERNEL_DATA_END__;
extern const uint8_t multiboot_data;
extern const uint8_t acpi_data;

// Import asm functions
extern "C" {
void load_page_directory(uint32_t*);
void enable_system_paging();
void bios_call();
void interrupt_return();
void start_first_thread(Kernel::Context *thread);
void switch_context(Kernel::Context **current, Kernel::Context **next);
[[noreturn]] void on_exception(uint32_t);
[[nodiscard]] int32_t is_cpuid_available();
void _init();
void _fini();
}

#endif