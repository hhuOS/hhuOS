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
 * The APIC implementation is based on a bachelor's thesis, written by Christoph Urlacher.
 * The original source code can be found here: https://github.com/ChUrl/hhuOS
 */

#include "ApicTimer.h"

#include "device/interrupt/apic/LocalApic.h"
#include "kernel/service/InterruptService.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/log/Log.h"
#include "kernel/service/Service.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Scheduler.h"
#include "kernel/service/TimeService.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

uint32_t ApicTimer::BASE_FREQUENCY = 0;

ApicTimer::ApicTimer(Util::Time::Timestamp timerInterval, Util::Time::Timestamp yieldInterval) : cpuId(LocalApic::getId()), timerInterval(timerInterval), yieldInterval(yieldInterval) {
    auto counter = (BASE_FREQUENCY / 1000) * timerInterval.toMilliseconds();
    LOG_INFO("Setting APIC timer [%u] interval to [%ums] (Counter: [%u])", cpuId, static_cast<uint32_t>(timerInterval.toMilliseconds()), static_cast<uint32_t>(counter));

    // Recommended order: Divide -> LVT -> Initial Count (OSDev)
    LocalApic::writeDoubleWord(LocalApic::TIMER_DIVIDE, Divider::BY_1);
    LocalApic::LocalVectorTableEntry lvtEntry = LocalApic::readLocalVectorTable(LocalApic::TIMER);
    lvtEntry.timerMode = LocalApic::LocalVectorTableEntry::TimerMode::PERIODIC;
    LocalApic::writeLocalVectorTable(LocalApic::TIMER, lvtEntry);
    LocalApic::writeDoubleWord(LocalApic::TIMER_INITIAL, counter);
}

void ApicTimer::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::APICTIMER, *this);
    LocalApic::allow(LocalApic::TIMER);
}

void ApicTimer::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    if (cpuId != LocalApic::getId()) {
        // Every core's timer uses the same (this) handler, but it exists once per core (each core has its own ApicTimer instance).
        // All handlers are registered to the same interrupt vector, we only want to reach the instance belonging to this core.
        return;
    }

    // Increase the "core-local" time, the system time is still managed by the PIT/HPET.
    time += timerInterval;

    if (cpuId != 0) {
        // Currently there is only one scheduler, it should get triggered only by the BSP.
        // Otherwise, the scheduler would be triggered n-times faster than intended, where n
        // is the CPU count.
        return;
    }

    timeSinceLastYield += timerInterval;
    if (timeSinceLastYield >= yieldInterval) {
        timeSinceLastYield.reset();
        // Currently there is only one main scheduler, for SMP systems this should yield the core scheduler or something similar.
        Kernel::Service::getService<Kernel::ProcessService>().getScheduler().yield(true);
    }
}

Util::Time::Timestamp ApicTimer::getTime() {
    return time;
}

void ApicTimer::calibrate() {
    if (BASE_FREQUENCY != 0) {
        return; // Timer is already calibrated
    }

    // Prepare calibration
    auto &timeService = Kernel::Service::getService<Kernel::TimeService>();
    LocalApic::writeDoubleWord(LocalApic::TIMER_DIVIDE, BY_1);
    LocalApic::LocalVectorTableEntry lvtEntry = LocalApic::readLocalVectorTable(LocalApic::TIMER);
    lvtEntry.timerMode = LocalApic::LocalVectorTableEntry::TimerMode::ONESHOT;
    LocalApic::writeLocalVectorTable(LocalApic::TIMER, lvtEntry);

    // The calibration works by waiting the desired interval and measuring how many ticks the timer does.
    auto waitTime = Util::Time::Timestamp::ofMilliseconds(100);
    LocalApic::writeDoubleWord(LocalApic::TIMER_INITIAL, 0xffffffff); // Max initial counter, writing starts timer
    timeService.busyWait(waitTime);
    BASE_FREQUENCY = (0xffffffff - LocalApic::readDoubleWord(LocalApic::TIMER_CURRENT)) * 10; // Ticks in 1 ms

    LOG_INFO("Apic Timer frequency: [%u MHz]", BASE_FREQUENCY / 1000000);
}

uint8_t ApicTimer::getCpuId() const {
    return cpuId;
}

}