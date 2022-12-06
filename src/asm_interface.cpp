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

#include "kernel/multiboot/Structure.h"
#include "kernel/system/System.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/paging/Paging.h"
#include "kernel/service/InterruptService.h"
#include "device/cpu/Cpu.h"
#include "GatesOfHell.h"
#include "device/power/acpi/Acpi.h"

// Import functions
extern "C" {
void _fini();
}

// Export functions
extern "C" {
void main();
void init_gdt(uint16_t*, uint16_t*, uint16_t*, uint16_t*, uint16_t*);
void copy_multiboot_info(Kernel::Multiboot::Info*, uint8_t*);
void copy_acpi_tables(uint8_t*);
void read_memory_map(Kernel::Multiboot::Info*);
void initialize_system(Kernel::Multiboot::Info*, uint8_t*);
void finish_system();
void bootstrap_paging(uint32_t*, uint32_t*);
void enable_interrupts();
void disable_interrupts();
void dispatch_interrupt(Kernel::InterruptFrame*);
void set_tss_stack_entry(uint32_t);
void flush_tss();
void release_scheduler_lock();
int32_t atexit (void (*func)()) noexcept;
}

void main() {
    Kernel::Logger::get("System").info("Entering Gates of Hell");
    GatesOfHell::enter();
}

void init_gdt(uint16_t *gdt, uint16_t *gdt_bios, uint16_t *gdt_descriptor, uint16_t *gdt_bios_descriptor, uint16_t *gdt_phys_descriptor) {
    Kernel::System::initializeGlobalDescriptorTables(gdt, gdt_bios, gdt_descriptor, gdt_bios_descriptor, gdt_phys_descriptor);
}

void copy_multiboot_info(Kernel::Multiboot::Info *source, uint8_t *destination) {
    Kernel::Multiboot::Structure::copyMultibootInfo(source, destination);
}

void copy_acpi_tables(uint8_t *destination) {
    Device::Acpi::copyAcpiTables(destination);
}

void read_memory_map(Kernel::Multiboot::Info *address) {
    Kernel::Multiboot::Structure::readMemoryMap(address);
}

void initialize_system(Kernel::Multiboot::Info *multibootAddress, uint8_t *acpiAddress) {
    Kernel::System::initializeSystem(multibootAddress, acpiAddress);
}

void finish_system() {
    _fini();
}

void bootstrap_paging(uint32_t *directory, uint32_t *biosDirectory) {
    Kernel::Paging::bootstrapPaging(directory, biosDirectory);
}

void enable_interrupts() {
    Device::Cpu::enableInterrupts();
}

void disable_interrupts() {
    Device::Cpu::disableInterrupts();
}

void dispatch_interrupt(Kernel::InterruptFrame *frame) {
    if (Kernel::System::isInitialized()) {
        Kernel::System::getService<Kernel::InterruptService>().dispatchInterrupt(*frame);
    } else {
        Kernel::System::handleEarlyInterrupt(*frame);
    }
}

void set_tss_stack_entry(uint32_t esp0) {
    Kernel::System::getTaskStateSegment().esp0 = esp0 + sizeof(Kernel::InterruptFrame);
    Kernel::System::getTaskStateSegment().ss0 = 0x10;
}

void release_scheduler_lock() {
    Kernel::System::getService<Kernel::SchedulerService>().unlockScheduler();
}

int32_t atexit (void (*func)()) noexcept {
    return 0;
}