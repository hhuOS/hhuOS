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


#include <kernel/memory/MemLayout.h>
#include <kernel/multiboot/Structure.h>
#include <util/memory/Address.h>
#include "Bios.h"

namespace Device {

// extern code in interrupt.asm
extern "C" { void bios_call(); }

// pointer to memory for parameters
const Bios::CallParameters *Bios::parameters = reinterpret_cast<const Bios::CallParameters*>(VIRT_BIOS16_PARAM_BASE);

bool Bios::isAvailable() {
    return Kernel::Multiboot::Structure::getKernelOption("bios") == "true";
}

void Bios::init() {
    if (!isAvailable()) {
        Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION,"BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    // pointer to memory segment for 16 bit code
    auto *codeAddress = reinterpret_cast<uint8_t *>(VIRT_BIOS16_CODE_MEMORY_START);

    // the assembler instructions are placed manually into the memory
    // in the following steps

    // mov eax, 5000
    *codeAddress++ = 0x66;
    *codeAddress++ = 0xB8;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x50;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x00;

    // mov [eax], esp
    *codeAddress++ = 0x66;
    *codeAddress++ = 0x67;
    *codeAddress++ = 0x89;
    *codeAddress++ = 0x20;

    // mov eax,cr0
    *codeAddress++ = 0x0F;
    *codeAddress++ = 0x20;
    *codeAddress++ = 0xC0;

    // and eax, 7FFEFFFE
    *codeAddress++ = 0x66;
    *codeAddress++ = 0x25;
    *codeAddress++ = 0xFE;
    *codeAddress++ = 0xFF;
    *codeAddress++ = 0xFE;
    *codeAddress++ = 0x7F;

    // mov cr0, eax
    *codeAddress++ = 0x0F;
    *codeAddress++ = 0x22;
    *codeAddress++ = 0xC0;

    // jmp 2400:001B flush pipeline & switch decoding unit
    // 0400:001B (0400<<4 = 4000 + 1B)
    *codeAddress++ = 0xEA;
    *codeAddress++ = 0x1B;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x04;

    // mov ds,400
    *codeAddress++ = 0xBA;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x04;

    // mov ss,dx
    *codeAddress++ = 0x8E;
    *codeAddress++ = 0xD2;

    // mov gs,dx
    *codeAddress++ = 0x8E;
    *codeAddress++ = 0xEA;

    // mov esp,2000 -> BIOS16_PARAM_BASE 0x6000
    *codeAddress++ = 0x66;
    *codeAddress++ = 0xBC;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x20;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x00;

    // pop ds
    *codeAddress++ = 0x1F;

    // pop es
    *codeAddress++ = 0x07;

    // pop fs
    *codeAddress++ = 0x0f;
    *codeAddress++ = 0xa1;

    // pop ax -> we have to pop something for symmetry
    *codeAddress++ = 0x58;

    // popad
    *codeAddress++ = 0x66;
    *codeAddress++ = 0x61;

    // interrupt number (written here)
    *codeAddress++ = 0xCD;
    *codeAddress++ = 0x00;

    // pushad
    *codeAddress++ = 0x66;
    *codeAddress++ = 0x60;

    // pushf
    *codeAddress++ = 0x9C;

    // push fs
    *codeAddress++ = 0x0f;
    *codeAddress++ = 0xa0;

    // push es
    *codeAddress++ = 0x06;

    // push ds
    *codeAddress++ = 0x1E;

    // mov eax,cr0
    *codeAddress++ = 0x0F;
    *codeAddress++ = 0x20;
    *codeAddress++ = 0xC0;

    // or eax, 00010001 (protected mode without paging)
    *codeAddress++ = 0x66;
    *codeAddress++ = 0x0D;
    *codeAddress++ = 0x01;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x01;
    *codeAddress++ = 0x00;

    // mov cr0, eax
    *codeAddress++ = 0x0F;
    *codeAddress++ = 0x22;
    *codeAddress++ = 0xC0;

    // jmp 0018:0049, flush pipeline & switch decoding
    // 0018:0049
    *codeAddress++ = 0xEA;
    *codeAddress++ = 0x49;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x18;
    *codeAddress++ = 0x00;

    // mov dx,0010
    *codeAddress++ = 0xBA;
    *codeAddress++ = 0x10;
    *codeAddress++ = 0x00;

    // mov ds,dx
    *codeAddress++ = 0x8E;
    *codeAddress++ = 0xDA;

    // mov es,dx
    *codeAddress++ = 0x8E;
    *codeAddress++ = 0xC2;

    // mov es,dx
    *codeAddress++ = 0x8E;
    *codeAddress++ = 0xE2;

    // mov fs,dx
    *codeAddress++ = 0x8E;
    *codeAddress++ = 0xEA;

    // mov ss,dx
    *codeAddress++ = 0x8E;
    *codeAddress++ = 0xD2;

    // mov eax, 25000
    *codeAddress++ = 0x66;
    *codeAddress++ = 0xB8;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x50;
    *codeAddress++ = 0x00;
    *codeAddress++ = 0x00;

    // mov esp, [eax]
    *codeAddress++ = 0x66;
    *codeAddress++ = 0x67;
    *codeAddress++ = 0x8B;
    *codeAddress++ = 0x20;

    // far ret
    *codeAddress++ = 0x66;
    *codeAddress++ = 0xCB;
}

void Bios::interrupt(int interruptNumber, CallParameters &callParameters) {
    if (!isAvailable()) {
        Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION,"BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    // Copy given parameters into the struct used by the 16-bit code
    Util::Memory::Address<uint32_t> source(&callParameters);
    Util::Memory::Address<uint32_t> target(parameters);
    target.copyRange(source, sizeof(CallParameters));

    // get pointer to bios call segment
    auto *ptr = reinterpret_cast<uint8_t*>(VIRT_BIOS16_CODE_MEMORY_START);
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
    source.copyRange(target, sizeof(CallParameters));;
}

}