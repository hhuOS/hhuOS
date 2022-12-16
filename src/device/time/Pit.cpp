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

#include "kernel/service/InterruptService.h"
#include "device/interrupt/Pic.h"
#include "Pit.h"
#include "kernel/system/System.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/log/Logger.h"
#include "kernel/service/SchedulerService.h"
#include "lib/util/Exception.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

Kernel::Logger Pit::log = Kernel::Logger::get("Pit");

Pit::Pit(uint16_t interruptRateDivisor, uint32_t yieldInterval) : yieldInterval(yieldInterval) {
    setInterruptRate(interruptRateDivisor);
}

void Pit::setInterruptRate(uint16_t divisor) {
    if (divisor == 0) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "PIT: Divisor may not be set to 0!");
    }

    auto interval = 1000000000 / (BASE_FREQUENCY / divisor);
    log.info("Setting PIT interval to [%uns] (Divisor: [%u])", interval, divisor);

    controlPort.writeByte(0x36); // Select channel 0, Use low-/high byte access mode, Set operating mode to rate generator
    dataPort0.writeByte((uint8_t) (divisor & 0xff)); // Low byte
    dataPort0.writeByte((uint8_t) (divisor >> 8)); // High byte
    timerInterval = interval;
}

void Pit::plugin() {
    auto &interruptService = Kernel::System::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptDispatcher::PIT, *this);
    interruptService.allowHardwareInterrupt(Pic::Interrupt::PIT);
}

void Pit::trigger(const Kernel::InterruptFrame &frame) {
    time.addNanoseconds(timerInterval);
    if (time.toMilliseconds() % yieldInterval == 0) {
        Kernel::System::getService<Kernel::SchedulerService>().yield();
    }
}

Util::Time::Timestamp Pit::getTime() {
    return time;
}

}