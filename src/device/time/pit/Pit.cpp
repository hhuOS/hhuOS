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
#include "device/system/FirmwareConfiguration.h"
#include "device/interrupt/InterruptRequest.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/service/Service.h"
#include "kernel/service/ProcessService.h"
#include "kernel/process/Scheduler.h"

namespace Kernel {
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

Pit::Pit(uint32_t timerInterval, uint32_t yieldInterval) : yieldInterval(yieldInterval) {
    setInterruptRate(timerInterval);
}

void Pit::setInterruptRate(uint32_t interval) {
    auto divisor = static_cast<uint32_t>(BASE_FREQUENCY * (interval / 1000.0));
    if (divisor > UINT16_MAX) divisor = UINT16_MAX;

    timerInterval = static_cast<uint32_t>(1000000000 / (static_cast<double>(BASE_FREQUENCY) / divisor));
    LOG_INFO("Setting PIT interval to [%ums] (Divisor: [%u])", timerInterval / 1000000 < 1 ? 1 : timerInterval / 1000000, divisor);

    // For some reason, the PIT interrupt rate is doubled, when it is attached to an IO APIC (only in QEMU)
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    if (FirmwareConfiguration::isAvailable() && interruptService.usesApic()) {
        divisor *= 2;
    }

    controlPort.writeByte(0x36); // Select channel 0, Use low-/high byte access mode, Set operating mode to rate generator
    dataPort0.writeByte((uint8_t) (divisor & 0xff)); // Low byte
    dataPort0.writeByte((uint8_t) (divisor >> 8)); // High byte
}

void Pit::plugin() {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    interruptService.assignInterrupt(Kernel::InterruptVector::PIT, *this);
    interruptService.allowHardwareInterrupt(Device::InterruptRequest::PIT);
}

void Pit::trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) {
    time.addNanoseconds(timerInterval);

    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    if (!interruptService.usesApic() && time.toMilliseconds() % yieldInterval == 0) {
        Kernel::Service::getService<Kernel::ProcessService>().getScheduler().yield();
    }
}

Util::Time::Timestamp Pit::getTime() {
    return time;
}

void Pit::earlyDelay(uint16_t ms) {
    const auto counter = static_cast<uint32_t>((BASE_FREQUENCY / 1000) * ms);
    const auto lowByte = static_cast<uint8_t>(counter & 0x00ff);
    const auto highByte = static_cast<uint8_t>((counter & 0xff00) >> 8);

    asm volatile (
            "mov $0x30, %%al;" // Channel 0, mode 0, low-/high byte access mode
            "outb $0x43;" // Control port

            // Set counter value
            "mov (%0), %%al;" // Low byte
            "outb $0x40;" // Data port
            "mov (%1), %%al;" // High byte
            "outb $0x40;" // Data port

            // Wait until output pin bit is set (counter reached 0)
            "pit_delay_loop:" // Loop label
            "mov $0xe2, %%al;" // Read status byte -> channel 0, mode 0, low-/high byte access mode
            "outb $0x43;" // Control port
            "inb $0x40;" // Data port
            "test $0x80, %%al;" // Test bit 7 (output pin state)
            "jz pit_delay_loop" // If bit 7 is not set -> loop
            : :
            "r"(&lowByte), "r"(&highByte)
            : "al"
            );
}

}