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

#include "ApicTimer.h"

#include "device/interrupt/apic/LocalApic.h"
#include "Pit.h"
#include "kernel/service/SchedulerService.h"
#include "kernel/system/System.h"
#include "kernel/service/InterruptService.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/log/Logger.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

uint32_t ApicTimer::ticksPerMilliseconds = 0;
ApicTimer::Divider ApicTimer::divider = BY_16;
Kernel::Logger ApicTimer::log = Kernel::Logger::get("APIC");

ApicTimer::ApicTimer(uint32_t timerInterval, uint32_t yieldInterval) : cpuId(LocalApic::getId()), timerInterval(timerInterval), yieldInterval(yieldInterval) {
    auto counter = ticksPerMilliseconds * timerInterval;
    // log.info("Setting APIC timer [%u] interval to [%ums] (Counter: [%u])", cpuId, timerInterval, counter);

    // Recommended order: Divide -> LVT -> Initial Count (OSDev)
    LocalApic::writeDoubleWord(LocalApic::TIMER_DIVIDE, divider); // BY_1 is the highest resolution (overkill)
    LocalApic::LocalVectorTableEntry lvtEntry = LocalApic::readLocalVectorTable(LocalApic::TIMER);
    lvtEntry.timerMode = LocalApic::LocalVectorTableEntry::TimerMode::PERIODIC;
    LocalApic::writeLocalVectorTable(LocalApic::TIMER, lvtEntry);
    LocalApic::writeDoubleWord(LocalApic::TIMER_INITIAL, counter);
}

void ApicTimer::plugin() {
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::APICTIMER, *this);
    LocalApic::allow(LocalApic::TIMER);
}

void ApicTimer::trigger(const Kernel::InterruptFrame &frame) {
    if (cpuId != LocalApic::getId()) {
        // Every core's timer uses the same (this) handler, but it exists once per core (each core has its own ApicTimer instance).
        // All handlers are registered to the same interrupt vector, we only want to reach the instance belonging to this core.
        return;
    }

    // Increase the "core-local" time, the system time is still managed by the PIT.
    time.addNanoseconds(timerInterval * 1000000); // Interval is in milliseconds

    if (cpuId != 0) {
        // Currently there is only one scheduler, it should get triggered only by the BSP.
        // Otherwise, the scheduler would be triggered n-times faster than intended, where n
        // is the CPU count.
        return;
    }

    if (time.toMilliseconds() % yieldInterval == 0) {
        // Currently there is only one main scheduler, for SMP systems this should yield the core scheduler or something similar.
        Kernel::System::getService<Kernel::SchedulerService>().yield();
    }
}

Util::Time::Timestamp ApicTimer::getTime() {
    return time;
}

void ApicTimer::calibrate() {
    // Prepare calibration
    LocalApic::writeDoubleWord(LocalApic::TIMER_DIVIDE, divider);
    LocalApic::LocalVectorTableEntry lvtEntry = LocalApic::readLocalVectorTable(LocalApic::TIMER);
    lvtEntry.timerMode = LocalApic::LocalVectorTableEntry::TimerMode::ONESHOT;
    LocalApic::writeLocalVectorTable(LocalApic::TIMER, lvtEntry);

    // The calibration works by waiting the desired interval and measuring how many ticks the timer does.
    LocalApic::writeDoubleWord(LocalApic::TIMER_INITIAL, 0xFFFFFFFF); // Max initial counter, writing starts timer
    Pit::earlyDelay(10000); // Wait 10 ms
    ticksPerMilliseconds = (0xFFFFFFFF - LocalApic::readDoubleWord(LocalApic::TIMER_CURRENT)) / 10; // Ticks in 1 ms

    log.info("Apic Timer ticks per millisecond: [%u]", ticksPerMilliseconds);
}

uint8_t ApicTimer::getCpuId() const {
    return cpuId;
}

}