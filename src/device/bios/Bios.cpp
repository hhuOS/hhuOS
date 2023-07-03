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
 */


#include "kernel/paging/MemoryLayout.h"
#include "kernel/multiboot/Multiboot.h"
#include "lib/util/base/Address.h"
#include "device/cpu/Cpu.h"
#include "kernel/system/System.h"
#include "lib/util/base/Exception.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/base/String.h"
#include "device/time/Cmos.h"
#include "kernel/service/InterruptService.h"
#include "device/bios/Bios.h"
#include "asm_interface.h"

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
    return Kernel::Multiboot::hasKernelOption("bios") && Kernel::Multiboot::getKernelOption("bios") == "true" && !Kernel::System::getService<Kernel::InterruptService>().usesApic();
}

void Bios::init() {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION,"BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    const auto sourceAddress = Util::Address<uint32_t>(reinterpret_cast<uint32_t>(&bios_call_16_start));
    const auto targetAddress = Kernel::MemoryLayout::BIOS_CALL_CODE_AREA.toVirtual().toAddress();
    const auto size = reinterpret_cast<uint32_t>(&bios_call_16_end) - reinterpret_cast<uint32_t>(&bios_call_16_start);
    targetAddress.copyRange(sourceAddress, size);

    const auto realModeIdtAddress = Kernel::MemoryLayout::BIOS_CALL_IDT.toVirtual().toAddress();
    realModeIdtAddress.setShort(0x3ff, 0);
    realModeIdtAddress.setInt(0, 2);
}

Bios::RealModeContext Bios::interrupt(int interruptNumber, const RealModeContext &context) {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "BIOS-calls are deactivated! Set 'bios=true', to activate them.");
    }

    interruptLock.acquire();
    // Get pointer to BIOS context inside lower memory
    auto *biosContext = reinterpret_cast<RealModeContext*>(Kernel::MemoryLayout::BIOS_CALL_STACK.toVirtual().startAddress);

    // Copy given context into lower memory
    *biosContext = context;

    // Write number of bios interrupt manually into code
    auto interruptNumberAddress = Kernel::MemoryLayout::BIOS_CALL_CODE_AREA.toVirtual().toAddress().add(reinterpret_cast<uint32_t>(&bios_call_16_interrupt) - reinterpret_cast<uint32_t>(&bios_call_16_start));
    interruptNumberAddress.setByte(interruptNumber, 1);

    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();

    // Disable interrupts during the bios call, since our protected mode handler cannot be called
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    // Save interrupt mask
    auto interruptMask = interruptService.getInterruptMask();

    // Call assembly code
    bios_call();

    // Restore interrupt mask
    interruptService.setInterruptMask(interruptMask);

    // Bios call has returned -> Enable interrupts
    Cmos::enableNmi();
    Cpu::enableInterrupts();

    // Return a copy of the BIOS context
    return interruptLock.releaseAndReturn(*biosContext);
}

}