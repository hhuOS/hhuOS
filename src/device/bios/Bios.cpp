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


#include <kernel/memory/MemLayout.h>
#include <kernel/multiboot/Structure.h>
#include <lib/util/memory/Address.h>
#include <asm_interface.h>
#include <device/cpu/Cpu.h>
#include "Bios.h"

namespace Device {

// pointer to memory for parameters
const Bios::CallParameters *Bios::parameters = reinterpret_cast<const Bios::CallParameters*>(VIRT_BIOS16_PARAM_BASE);

bool Bios::isAvailable() {
    return Kernel::Multiboot::Structure::getKernelOption("bios") == "true";
}

void Bios::init() {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    // Address to memory segment for 16 bit code
    const auto codeAddress = Util::Memory::Address<uint32_t>(VIRT_BIOS16_CODE_MEMORY_START);
    uint32_t offset = 0;

    // the assembler instructions are placed manually into the memory in the following steps
    // mov eax, 5000
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xB8, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x50, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov [eax], esp
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x67, offset++);
    codeAddress.setByte(0x89, offset++);
    codeAddress.setByte(0x20, offset++);

    // mov eax,cr0
    codeAddress.setByte(0x0F, offset++);
    codeAddress.setByte(0x20, offset++);
    codeAddress.setByte(0xC0, offset++);

    // and eax, 7FFEFFFE
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x25, offset++);
    codeAddress.setByte(0xFE, offset++);
    codeAddress.setByte(0xFF, offset++);
    codeAddress.setByte(0xFE, offset++);
    codeAddress.setByte(0x7F, offset++);

    // mov cr0, eax
    codeAddress.setByte(0x0F, offset++);
    codeAddress.setByte(0x22, offset++);
    codeAddress.setByte(0xC0, offset++);

    // jmp 2400:001B flush pipeline & switch decoding unit
    // 0400:001B (0400<<4 = 4000 + 1B)
    codeAddress.setByte(0xEA, offset++);
    codeAddress.setByte(0x1B, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x04, offset++);

    // mov ds,400
    codeAddress.setByte(0xBA, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x04, offset++);

    // mov ss,dx
    codeAddress.setByte(0x8E, offset++);
    codeAddress.setByte(0xD2, offset++);

    // mov gs,dx
    codeAddress.setByte(0x8E, offset++);
    codeAddress.setByte(0xEA, offset++);

    // mov esp,2000 -> BIOS16_PARAM_BASE 0x6000
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xBC, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x20, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);

    // pop ds
    codeAddress.setByte(0x1F, offset++);

    // pop es
    codeAddress.setByte(0x07, offset++);

    // pop fs
    codeAddress.setByte(0x0f, offset++);
    codeAddress.setByte(0xa1, offset++);

    // pop ax -> we have to pop something for symmetry
    codeAddress.setByte(0x58, offset++);

    // popad
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x61, offset++);

    // interrupt number (written here)
    codeAddress.setByte(0xCD, offset++);
    codeAddress.setByte(0x00, offset++);

    // pushad
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x60, offset++);

    // pushf
    codeAddress.setByte(0x9C, offset++);

    // push fs
    codeAddress.setByte(0x0f, offset++);
    codeAddress.setByte(0xa0, offset++);

    // push es
    codeAddress.setByte(0x06, offset++);

    // push ds
    codeAddress.setByte(0x1E, offset++);

    // mov eax,cr0
    codeAddress.setByte(0x0F, offset++);
    codeAddress.setByte(0x20, offset++);
    codeAddress.setByte(0xC0, offset++);

    // or eax, 00010001 (protected mode without paging)
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x0D, offset++);
    codeAddress.setByte(0x01, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x01, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov cr0, eax
    codeAddress.setByte(0x0F, offset++);
    codeAddress.setByte(0x22, offset++);
    codeAddress.setByte(0xC0, offset++);

    // jmp 0018:0049, flush pipeline & switch decoding
    // 0018:0049
    codeAddress.setByte(0xEA, offset++);
    codeAddress.setByte(0x49, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x18, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov dx,0010
    codeAddress.setByte(0xBA, offset++);
    codeAddress.setByte(0x10, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov ds,dx
    codeAddress.setByte(0x8E, offset++);
    codeAddress.setByte(0xDA, offset++);

    // mov es,dx
    codeAddress.setByte(0x8E, offset++);
    codeAddress.setByte(0xC2, offset++);

    // mov es,dx
    codeAddress.setByte(0x8E, offset++);
    codeAddress.setByte(0xE2, offset++);

    // mov fs,dx
    codeAddress.setByte(0x8E, offset++);
    codeAddress.setByte(0xEA, offset++);

    // mov ss,dx
    codeAddress.setByte(0x8E, offset++);
    codeAddress.setByte(0xD2, offset++);

    // mov eax, 25000
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xB8, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x50, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov esp, [eax]
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x67, offset++);
    codeAddress.setByte(0x8B, offset++);
    codeAddress.setByte(0x20, offset++);

    // far ret
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xCB, offset);
}

void Bios::interrupt(int interruptNumber, const CallParameters &callParameters) {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    // Copy given parameters into the struct used by the 16-bit code
    Util::Memory::Address<uint32_t> source(&callParameters);
    Util::Memory::Address<uint32_t> target(parameters);
    target.copyRange(source, sizeof(CallParameters));

    // get pointer to bios call segment
    auto ptr = reinterpret_cast<uint8_t*>(VIRT_BIOS16_CODE_MEMORY_START);
    // write number of bios interrupt manually into the segment
    *(ptr + 48) = static_cast<uint8_t>(interruptNumber);

    // no interrupts during the bios call
    Cpu::disableInterrupts();
    // jump into assembler code
    bios_call();
    // bios call is returned, interrupts are allowed now
    Cpu::enableInterrupts();

    // Copy the struct used by the 16-bit code back into the given parameters
    // This way, it is possible to pass return values via cpu registers from real mode to protected mode
    source.copyRange(target, sizeof(CallParameters));
}

}