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


#include "kernel/paging/MemoryLayout.h"
#include "kernel/multiboot/Multiboot.h"
#include "lib/util/memory/Address.h"
#include "device/cpu/Cpu.h"
#include "asm_interface.h"
#include "Bios.h"
#include "kernel/system/System.h"
#include "kernel/service/MemoryService.h"
#include "lib/util/Exception.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/memory/String.h"

namespace Device {

Util::Async::Spinlock Bios::interruptLock;

uint16_t Bios::construct16BitRegister(uint8_t lowerValue, uint8_t higherValue) {
    return lowerValue | (higherValue << 8);
}

uint8_t Bios::get8BitRegister(uint16_t value, Bios::RegisterHalf half) {
    if (half == LOWER) {
        return value & 0x00ff;
    } else if (half == HIGHER) {
        return (value & 0xff00) >> 8;
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Bios: Invalid register half!");
}

bool Bios::isAvailable() {
    return Kernel::Multiboot::hasKernelOption("bios") && Kernel::Multiboot::getKernelOption("bios") == "true";
}

void Bios::init() {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    // Address to memory segment for 16 bit code
    const auto codeAddress = Kernel::MemoryLayout::BIOS_CODE_MEMORY.toVirtual().toAddress();
    uint32_t offset = 0;

    // The following assembly instructions are manually placed into the memory starting at 0x0500 (0x500 - 0x5fff is reserved for BIOS code)
    /***************************************************************************************************************************************
     * ; Save esp to 0x0ffc (last 4 bytes of memory reserved for the 16-bit code code)
     * mov [0x0ffc],esp
     *
     * ; Turn of protected mode via cr0 (also disable write protection)
     * mov eax,cr0
     * and eax,7ffefffe
     * mov cr0,eax
     *
     * ; Flush pipeline and switch decoding unit to real mode by performing a far jump to the next instruction
     * ; 0x0050:0x0016 = (0x0050 << 4) + 0x0016 = 0x0500 + 0x001b = 0x0516
     * jmp 0x0050:0x0016
     *
     * ; Setup segment registers and let the stack segment point directly to the context struct
     * ; The value 0x0000 is replaced manually with the appropriate segment address by Bios::interrupt()
     * mov dx,0x0000
     * mov ss,dx
     *
     * ; Set esp to 0x0000 (no offset needed, since ss already points exactly to the context struct)
     * mov esp,0x0000
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
     * ; 0x0018:0x0042 = GDT[0x0018] + 0x0042 = 0x0500 + 0x0042 = 0x0542
     * jmp 0x0018:0x0042
     *
     * ; Restore segment registers
     * mov dx,0x0010
     * mov ds,dx
     * mov es,dx
     * mov fs,dx
     * mov ss,dx
     *
     * ; Restore esp from 0x0ffc
     * mov esp,[0x0ffc]
     *
     * ; Far return to bios.asm
     * far ret
     **************************************************************************************************************************************/

    // mov [0x0ffc],esp
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x89, offset++);
    codeAddress.setByte(0x26, offset++);
    codeAddress.setByte(0xfc, offset++);
    codeAddress.setByte(0x0f, offset++);

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

    // jmp 0x0050:0x0016
    codeAddress.setByte(0xea, offset++);
    codeAddress.setByte(0x16, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x50, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov dx,0x0000
    codeAddress.setByte(0xba, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);

    // mov ss,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xd2, offset++);

    // mov esp,0x0000
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xbc, offset++);
    codeAddress.setByte(0x00, offset++);
    codeAddress.setByte(0x00, offset++);
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

    // jmp 0x0018:0x0042
    codeAddress.setByte(0xea, offset++);
    codeAddress.setByte(0x42, offset++);
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

    // mov ss,dx
    codeAddress.setByte(0x8e, offset++);
    codeAddress.setByte(0xd2, offset++);

    // mov esp,[0x0ffc]
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0x8b, offset++);
    codeAddress.setByte(0x26, offset++);
    codeAddress.setByte(0xfc, offset++);
    codeAddress.setByte(0x0f, offset++);

    // far ret
    codeAddress.setByte(0x66, offset++);
    codeAddress.setByte(0xcb, offset);
}

Bios::RealModeContext Bios::interrupt(int interruptNumber, const RealModeContext &context) {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    interruptLock.acquire();
    // Allocate space for BIOS context inside the lower memory
    auto *biosContext = static_cast<RealModeContext*>(Kernel::System::getService<Kernel::MemoryService>().allocateLowerMemory(sizeof(RealModeContext), 16));

    // Copy given context into the allocated space
    Util::Memory::Address<uint32_t> source(&context);
    Util::Memory::Address<uint32_t> target(biosContext);
    target.copyRange(source, sizeof(RealModeContext));

    // Get pointer to bios call segment
    auto biosCode = reinterpret_cast<uint8_t*>(Kernel::MemoryLayout::BIOS_CODE_MEMORY.toVirtual().startAddress);
    // Write number of bios interrupt manually into code
    *(biosCode + BIOS_INTERRUPT_NUMBER_ADDRESS_OFFSET) = static_cast<uint8_t>(interruptNumber);
    // Write stack segment address manually into code
    *reinterpret_cast<uint16_t*>(biosCode + STACK_SEGMENT_ADDRESS_OFFSET) = Kernel::MemoryLayout::VIRTUAL_TO_PHYSICAL(reinterpret_cast<uint32_t>(biosContext)) >> 4;

    // Disable interrupts during the bios call, since our protected mode handler cannot be called
    Cpu::disableInterrupts();
    // Call assembly code
    bios_call();
    // Bios call has returned -> Enable interrupts
    Cpu::enableInterrupts();

    // Create a copy of the BIOS context and free the allocated space in lower memory
    RealModeContext ret = *biosContext;
    Kernel::System::getService<Kernel::MemoryService>().freeLowerMemory(biosContext, 16);

    interruptLock.release();
    return ret;
}

}