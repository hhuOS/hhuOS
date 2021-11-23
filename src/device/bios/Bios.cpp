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
const Bios::CallParameters *Bios::parameters = reinterpret_cast<const Bios::CallParameters*>(Kernel::MemoryLayout::VIRT_BIOS16_PARAM_BASE);

bool Bios::isAvailable() {
    return Kernel::Multiboot::Structure::getKernelOption("bios") == "true";
}

void Bios::init() {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    // Address to memory segment for 16 bit code
    const auto codeAddress = Util::Memory::Address<uint32_t>(Kernel::MemoryLayout::VIRT_BIOS16_CODE_MEMORY_START);
    uint32_t offset = 0;

    // The following assembly instructions are manually placed into the memory starting at 0x4000
    /***************************************************************************************************************************************
     * ; Save esp to 0x5000
     * mov eax,5000
     * mov [eax],esp
     *
     * ; Turn of protected mode via cr0 (also disable write protection)
     * mov eax,cr0
     * and eax,7ffefffe
     * mov cr0,eax
     *
     * ; Flush pipeline and switch decoding unit to real mode by performing a far jump to the next instruction
     * ; 0x0400:0x001b = (0x0400 << 4) + 0x001b = 0x4000 + 0x001b = 0x401b
     * jmp 0x0400:0x001b
     *
     * ; Setup segment registers
     * mov dx,0x0400
     * mov ss,dx
     * mov gs,dx
     *
     * ; Set esp to the parameter struct
     * ; 0x6000 = 0x4000 (segment base) + 0x2000 (offset)
     * mov esp,0x2000
     *
     * ; Pop parameters from the struct (now on stack) into the CPU registers
     * pop ds
     * pop es
     * pop fs
     * pop ax   ; Only to skip flags
     * popad
     *
     * ; Perform BIOS call (value is manually placed into memory by Bios::interrupt())
     * int 0x00
     *
     * ; Push registers into the parameter struct, which then holds return values from the BIOS call
     * pushad
     * pushf    ; Do not skip flags this time
     * push fs
     * push es
     * push ds
     *
     * ; Enable protected mode (without paging) and write protection
     * mov eax,cr0
     * or eax,0x00010001
     * mov cr0,eax
     *
     * ; Flush pipeline and switch decoding unit to protected mode by performing a far jump to the next instruction
     * ; 0x0018:0x0049 = GDT[0x0018] + 0x0049 = 0x4000 + 0x0049 = 0x4049
     * jmp 0x0018:0x0049
     *
     * ; Setup segment registers
     * mov dx,0x0010
     * mov ds,dx
     * mov es,dx
     * mov fs,dx
     * mov gs,dx
     * mov ss,dx
     *
     * ; Restore esp from 0x5000
     * mov eax,0x5000
     * mov esp,[eax]
     *
     * ; Far return to bios.asm
     * far ret
     **************************************************************************************************************************************/

    // mov eax,0x5000
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xb8, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x50, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov [eax],esp
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x67, offset++);
    codeAddress.setByte(0x89, offset++);
    codeAddress.setByte(0x20, offset++);

    // mov eax,cr0
    codeAddress.setByte(0x0f, offset++);
    codeAddress.setByte(0x20, offset++);
    codeAddress.setByte(0xc0, offset++);

    // and eax,0x7ffefffe
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x25, offset++);
    codeAddress.setByte(0xfe, offset++);
    codeAddress.setByte(0xff, offset++);
    codeAddress.setByte(0xfe, offset++);
    codeAddress.setByte(0x7f, offset++);

    // mov cr0,eax
    codeAddress.setByte(0x0f, offset++);
    codeAddress.setByte(0x22, offset++);
    codeAddress.setByte(0xc0, offset++);

    // jmp 0400:001b
    codeAddress.setByte(0xea, offset++);
    codeAddress.setByte(0x1b, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x04, offset++);

    // mov dx,0x0400
    codeAddress.setByte(0xba, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x04, offset++);

    // mov ss,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xd2, offset++);

    // mov gs,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xea, offset++);

    // mov esp,0x2000
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xbc, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x20, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);

    // pop ds
    codeAddress.setByte(0x1f, offset++);

    // pop es
    codeAddress.setByte(0x07, offset++);

    // pop fs
    codeAddress.setByte(0x0f, offset++);
    codeAddress.setByte(0xa1, offset++);

    // pop ax
    codeAddress.setByte(0x58, offset++);

    // popad
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x61, offset++);

    // int 0x00
    codeAddress.setByte(0xcd, offset++);
    codeAddress.setByte(0x00, offset++);

    // pushad
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x60, offset++);

    // pushf
    codeAddress.setByte(0x9c, offset++);

    // push fs
    codeAddress.setByte(0x0f, offset++);
    codeAddress.setByte(0xa0, offset++);

    // push es
    codeAddress.setByte(0x06, offset++);

    // push ds
    codeAddress.setByte(0x1e, offset++);

    // mov eax,cr0
    codeAddress.setByte(0x0f, offset++);
    codeAddress.setByte(0x20, offset++);
    codeAddress.setByte(0xc0, offset++);

    // or eax,0x00010001
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x0d, offset++);
    codeAddress.setByte(0x01, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x01, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov cr0,eax
    codeAddress.setByte(0x0f, offset++);
    codeAddress.setByte(0x22, offset++);
    codeAddress.setByte(0xc0, offset++);

    // jmp 0x0018:0x0049
    codeAddress.setByte(0xea, offset++);
    codeAddress.setByte(0x49, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x18, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov dx,0x0010
    codeAddress.setByte(0xba, offset++);
    codeAddress.setByte(0x10, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov ds,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xda, offset++);

    // mov es,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xc2, offset++);

    // mov fs,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xe2, offset++);

    // mov gs,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xea, offset++);

    // mov ss,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xd2, offset++);

    // mov eax,0x5000
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xb8, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x50, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov esp,[eax]
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x67, offset++);
    codeAddress.setByte(0x8b, offset++);
    codeAddress.setByte(0x20, offset++);

    // far ret
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xcb, offset);
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
    auto biosCode = reinterpret_cast<uint8_t*>(Kernel::MemoryLayout::VIRT_BIOS16_CODE_MEMORY_START);
    // write number of bios interrupt manually into the segment
    *(biosCode + BIOS_INTERRUPT_NUMBER_ADDRESS_OFFSET) = static_cast<uint8_t>(interruptNumber);

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