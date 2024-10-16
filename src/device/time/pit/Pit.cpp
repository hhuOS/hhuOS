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
 */

#include "kernel/service/InterruptService.h"
#include "Pit.h"
#include "kernel/log/Log.h"
#include "device/interrupt/InterruptRequest.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/service/Service.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Scheduler.h"
#include "lib/util/async/Atomic.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

void Pit::disable() {
    auto controlPort = IoPort(0x43);
    auto command = Command(OperatingMode::INTERRUPT_ON_TERMINAL_COUNT, AccessMode::LOW_BYTE_ONLY);

    controlPort.writeByte(static_cast<uint8_t>(command));
}

void Pit::setInterruptRate(const Util::Time::Timestamp &timerInterval, const Util::Time::Timestamp &yieldInterval) {
    Pit::timerInterval = timerInterval;
    Pit::yieldInterval = yieldInterval;

    auto divisor = timerInterval.toNanoseconds() / NANOSECONDS_PER_TICK;
    if (divisor > UINT16_MAX) divisor = UINT16_MAX;

    setDivisor(divisor);
}

void Pit::setDivisor(uint16_t divisor) {
    timerInterval = Util::Time::Timestamp::ofNanoseconds(NANOSECONDS_PER_TICK * divisor);
    LOG_INFO("Setting PIT interval to [%ums] (Divisor: [%u])", static_cast<uint32_t>(timerInterval.toMilliseconds() < 1 ? 1 : timerInterval.toMilliseconds()), divisor);

    auto command = Command(OperatingMode::RATE_GENERATOR, AccessMode::LOW_BYTE_HIGH_BYTE);
    controlPort.writeByte(static_cast<uint8_t>(command)); // Select channel 0, Use low-/high byte access mode, Set operating mode to rate generator
    dataPort0.writeByte((uint8_t) (divisor & 0xff)); // Low byte
    dataPort0.writeByte((uint8_t) (divisor >> 8)); // High byte
}

void Pit::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::PIT, *this);
    interruptService.allowHardwareInterrupt(Device::InterruptRequest::PIT);
}

void Pit::trigger([[maybe_unused]] const Kernel::InterruptFrame &frame, [[maybe_unused]] Kernel::InterruptVector slot) {
    intervals++;

    if (readerCount == 0) {
        while (intervals > 0) {
            time += timerInterval;
            intervals--;
        }
    }

    if (!Kernel::Service::getService<Kernel::InterruptService>().usesApic()) {
        timeSinceLastYield += timerInterval;
        if (timeSinceLastYield > yieldInterval) {
            timeSinceLastYield.reset();
            Kernel::Service::getService<Kernel::ProcessService>().getScheduler().yield(true);
        }
    }
}

Util::Time::Timestamp Pit::getTime() {
    auto readerCountAtomic = Util::Async::Atomic<uint32_t>(readerCount);

    readerCountAtomic.inc();
    auto ret = time;
    readerCountAtomic.dec();

    return ret;
}

void Pit::wait(const Util::Time::Timestamp &waitTime) {
    auto elapsedTime = Util::Time::Timestamp();
    auto lastTimerValue = readTimer();

    while (elapsedTime < waitTime) {
        auto timerValue = readTimer();
        auto ticks = lastTimerValue >= timerValue ? lastTimerValue - timerValue : ((timerInterval.toNanoseconds() / NANOSECONDS_PER_TICK) - timerValue + lastTimerValue);
        elapsedTime += Util::Time::Timestamp::ofNanoseconds(ticks * NANOSECONDS_PER_TICK);

        lastTimerValue = timerValue;
    }
}

bool Pit::isLocked() const {
    return readTimerLock.isLocked();
}

uint16_t Pit::readTimer() {
    const auto latchCountCommand = Command(OperatingMode::INTERRUPT_ON_TERMINAL_COUNT, AccessMode::LATCH_COUNT);

    readTimerLock.acquire();
    controlPort.writeByte(static_cast<uint8_t>(latchCountCommand));
    uint16_t lowByte = dataPort0.readByte();
    uint16_t highByte = dataPort0.readByte();
    readTimerLock.release();

    return lowByte | (highByte << 8);
}

Pit::Command::Command(Pit::OperatingMode operatingMode, Pit::AccessMode accessMode) : bcdBinaryMode(BINARY), operatingMode(operatingMode), accessMode(accessMode), channel(CHANNEL_0) {}

Pit::Command::operator uint8_t() const {
    return *reinterpret_cast<const uint8_t*>(this);
}

}