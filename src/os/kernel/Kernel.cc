/*
 * Copyright (C) 2018  Filip Krakowski, Fabian Ruhland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <devices/graphics/text/CgaText.h>
#include <lib/libc/printf.h>
#include "Kernel.h"
#include "KernelSymbols.h"
#include "kernel/Cpu.h"
#include "Bios.h"

Spinlock Kernel::serviceLock;

Util::HashMap<String, KernelService*> Kernel::serviceMap(SERVICE_MAP_SIZE);

void Kernel::registerService(const String &serviceId, KernelService* const &kernelService) {

    serviceLock.acquire();

    serviceMap.put(serviceId, kernelService);

    serviceLock.release();
}

bool Kernel::isServiceRegistered(const String &serviceId) {

    return serviceMap.containsKey(serviceId);
}

void Kernel::panic(InterruptFrame *frame) {

    Cpu::disableInterrupts();

    uint32_t slot = frame->interrupt;

    // initialize a CGA text for panic because it is the
    // most save way for outprints
    CgaText cgaText;

    // Switch to CGA-mode manually, because TextDriver::init() may try to allocate data on the heap.
    BC_params->AX = 0x03;
    Bios::Int(0x10);

    BC_params->AX = 0x0100;
    BC_params->CX = 0x2607;
    Bios::Int(0x10);

    // Paint screen blue
    auto *dest = (uint64_t *) VIRT_CGA_START;

    uint64_t end = cgaText.getRowCount() * cgaText.getColumnCount() * 2 / sizeof(uint64_t);

    for(uint64_t i = 0; i < end; i++) {
        dest[i] = 0x1000100010001000;
    }

    cgaText.setpos(0, 0);

    stdout = &cgaText;

    // print out the "bluescreen"

    printf("\n\n  [PANIC] %s\n\n", Cpu::getExceptionName(slot));

    printStacktrace(frame->ebp, frame->eip);

    printRegisters(*frame);

    // do not execute any further
    Cpu::halt();
}

void Kernel::printStacktrace(uint32_t basePointer, uint32_t instructionPointer) {

        uint32_t *ebp = (uint32_t*) basePointer;

        uint32_t eip = instructionPointer;

        uint32_t i = 0;

        while (eip) {

            printf("     #%02d 0x%08x --- %s\n", i, eip, (char*) KernelSymbols::get(eip));

            eip = ebp[1];

            ebp = (uint32_t*) ebp[0];

            if ((uint32_t ) ebp < KERNEL_START) {

                break;
            }

            i++;
        }
}

void Kernel::printRegisters(const InterruptFrame &frame) {

    printf("\n\n");
    printf("     eax=0x%08x  ebx=0x%08x  ecx=0x%08x  edx=0x%08x\n", frame.eax, frame.ebx, frame.ecx, frame.edx);
    printf("     esp=0x%08x  ebp=0x%08x  esi=0x%08x  edi=0x%08x\n\n", frame.esp, frame.ebp, frame.esi, frame.edi);
    printf("     eflags=0x%08x", frame.eflags);
}
