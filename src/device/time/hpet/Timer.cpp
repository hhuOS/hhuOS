/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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
 * The HPET driver is based on a bachelor's thesis, written by Suratsch Hassan.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-suhas102
 */

#include "Timer.h"

#include "Hpet.h"
#include "lib/util/base/Exception.h"
#include "kernel/service/InterruptService.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/async/Runnable.h"
#include "kernel/service/Service.h"

namespace Kernel {
enum GlobalSystemInterrupt : uint32_t;
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

Timer::Timer(Hpet &hpet, uint8_t id, InterruptRequest interrupt) : hpet(hpet), id(id) {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    Timer::interrupt = interruptService.getInterruptTarget(interrupt);

    auto config = hpet.readRegister(getRegisterOffset(Hpet::TIMER_CONFIGURATION_AND_CAPABILITIES));
    if (!(config & (1ull << (32 + Timer::interrupt)))) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "HPET timer does not support selected interrupt!");
    }
}

Util::Array<InterruptRequest> Timer::getValidInterruptLines(Hpet &hpet, uint8_t id) {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    auto config = hpet.readRegister(Hpet::TIMER_CONFIGURATION_AND_CAPABILITIES + 0x20 * id);
    auto interrupts = config >> 32;

    Util::ArrayList<InterruptRequest> validInterrupts;
    for (uint32_t i = 0; i < 32; i++) {
        if (interrupts & (1 << i)) {
            // HPET config stores IO-APIC interrupt lines, which may be remapped -> We need to convert these to the source interrupts for use with the interrupt service
            auto interrupt = interruptService.getInterruptSource(static_cast<Kernel::GlobalSystemInterrupt>(i));
            validInterrupts.add(interrupt);
        }
    }

    return validInterrupts.toArray();
}

void Timer::arm(uint64_t comparatorValue, Util::Async::Runnable &onInterrupt) {
    Timer::onInterrupt = &onInterrupt;

    auto config = hpet.readRegister(getRegisterOffset(Hpet::TIMER_CONFIGURATION_AND_CAPABILITIES));
    config |= INTERRUPT_ENABLE | (interrupt << 9);

    hpet.writeRegister(getRegisterOffset(Hpet::TIMER_CONFIGURATION_AND_CAPABILITIES), config);
    hpet.writeRegister(getRegisterOffset(Hpet::TIMER_COMPARATOR_VALUE), comparatorValue);
}

void Timer::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    auto interrupt = interruptService.getInterruptSource(Timer::interrupt);
    interruptService.assignInterrupt(static_cast<Kernel::InterruptVector>(interrupt + 32), *this);
    interruptService.allowHardwareInterrupt(interrupt);
}

void Timer::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    onInterrupt->run();
}

uint16_t Timer::getRegisterOffset(uint16_t offset) const {
    return offset + 0x20 * id;
}

}