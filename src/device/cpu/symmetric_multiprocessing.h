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
 *
 * The APIC implementation is based on a bachelor's thesis, written by Christoph Urlacher.
 * The original source code can be found here: https://github.com/ChUrl/hhuOS
 */

#ifndef HHUOS_SYMMETRIC_MULTIPROCESSING_H
#define HHUOS_SYMMETRIC_MULTIPROCESSING_H

#include <cstdint>
#include "Cpu.h"

// Import from symmetric_multiprocessing.asm
extern "C" void boot_ap(void);
extern const uint16_t boot_ap_size;
extern Device::Cpu::Descriptor boot_ap_idtr;
extern uint32_t boot_ap_cr0;
extern uint32_t boot_ap_cr3;
extern uint32_t boot_ap_cr4;
extern volatile uint32_t boot_ap_counter;
extern volatile uint32_t boot_ap_gdts;   // Not written by asm volatile (), so add volatile here
extern volatile uint32_t boot_ap_stacks; // Not written by asm volatile (), so add volatile here
extern volatile uint32_t boot_ap_entry;  // Not written by asm volatile (), so add volatile here

namespace Device {

// Export to symmetric_multiprocessing.asm
extern "C" [[noreturn]] void applicationProcessorEntry(uint8_t initializedApplicationProcessorsCounter);
extern "C" volatile bool runningApplicationProcessors[256];

// If any of these two are changed, smp.asm has to be changed too (the %defines at the top)!
const constexpr uint32_t applicationProcessorStackSize = 0x1000;      // Size of the stack allocated for each AP.
const constexpr uint16_t applicationProcessorStartupAddress = 0x8000; // Physical address the AP startup routine is copied to.

}

#endif
