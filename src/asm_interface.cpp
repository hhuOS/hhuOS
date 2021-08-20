/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#include <kernel/multiboot/Structure.h>
#include <kernel/core/Management.h>
#include <kernel/interrupt/InterruptDispatcher.h>
#include <kernel/memory/Paging.h>
#include <device/cpu/Cpu.h>
#include "GatesOfHell.h"

// Import functions
extern "C" {
void _fini();
}

// Export functions
extern "C" {
void main();
void init_gdt(uint16_t*, uint16_t*, uint16_t*, uint16_t*, uint16_t*);
void copy_multiboot_info(Kernel::Multiboot::Info*, uint8_t*, uint32_t);
void read_memory_map(Kernel::Multiboot::Info*);
void initialize_system(Kernel::Multiboot::Info*);
void finish_system();
void bootstrap_paging(uint32_t*, uint32_t*);
void enable_interrupts();
void disable_interrupts();
void dispatch_interrupt(Kernel::InterruptFrame*);
void set_tss_stack_entry(uint32_t);
int32_t atexit (void (*func)()) noexcept;
}

void main() {
    Kernel::Logger::get("Management").info("Entering Gates Of Hell");
    GatesOfHell::enter();
}

void init_gdt(uint16_t *gdt, uint16_t *gdt_bios, uint16_t *gdt_descriptor, uint16_t *gdt_bios_descriptor, uint16_t *gdt_phys_descriptor) {
    Kernel::Management::initializeGlobalDescriptorTables(gdt, gdt_bios, gdt_descriptor, gdt_bios_descriptor, gdt_phys_descriptor);
}

void copy_multiboot_info(Kernel::Multiboot::Info *source, uint8_t *destination, uint32_t max_bytes) {
    Kernel::Multiboot::Structure::copyMultibootInfo(source, destination, max_bytes);
}

void read_memory_map(Kernel::Multiboot::Info *address) {
    Kernel::Multiboot::Structure::readMemoryMap(address);
}

void initialize_system(Kernel::Multiboot::Info *address) {
    Kernel::Management::initializeSystem(address);
}

void finish_system() {
    _fini();
}

void bootstrap_paging(uint32_t *directory, uint32_t *biosDirectory) {
    Kernel::bootstrapPaging(directory, biosDirectory);
}

void enable_interrupts() {
    Device::Cpu::enableInterrupts();
}

void disable_interrupts() {
    Device::Cpu::disableInterrupts();
}

void dispatch_interrupt(Kernel::InterruptFrame *frame) {
    Kernel::InterruptDispatcher::getInstance().dispatch(frame);
}

void set_tss_stack_entry(uint32_t esp0) {
    Kernel::Management::getTaskStateSegment().esp0 = esp0 + sizeof(Kernel::InterruptFrame);
}

int32_t atexit (void (*func)()) noexcept {
    return 0;
}