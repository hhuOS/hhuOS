/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_APICTIMER_H
#define HHUOS_APICTIMER_H

#include <stdint.h>

#include "kernel/interrupt/InterruptHandler.h"
#include "device/time/TimeProvider.h"
#include "lib/util/time/Timestamp.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

/**
 * This class implements the APIC timer device.
 *
 * Its purpose is to handlePageFault per-core scheduler preemption in SMP systems, although it is also used
 * in single core systems. It is not used for system-time keeping, this is still done by the PIT.
 *
 * It receives its tick interval in milliseconds, which should be precise enough for scheduling.
 * If a more precise interval is required, the timer divider might need adjustment.
 */
class ApicTimer : public Kernel::InterruptHandler, public TimeProvider {

public:
    /**
     * These are the different divider mode the APIC timer's counter supports.
     *
     * The APIC timer generates signals of a certain frequency by counting down a register.
     * If the divider is set to "BY_1", the register is counted down on every bus clock, yielding
     * the highest precision. If this is not required, the countdown can be slowed by dividing with
     * a higher value. This can achieve longer intervals that would otherwise not be possible with
     * a 32-bit counter register. See IA-32 manual, sec. 3.11.5.4
     */
    enum Divider : uint32_t {
        BY_1 = 0b1011,
        BY_2 = 0b0000,
        BY_4 = 0b0001,
        BY_8 = 0b0010,
        BY_16 = 0b0011,
        BY_32 = 0b1000,
        BY_64 = 0b1001,
        BY_128 = 0b1010
    };

    /**
     * Constructor.
     *
     * @param timerInterval The tick interval in milliseconds (10 milliseconds by default)
     * @param yieldInterval The preemption interval in milliseconds (10 milliseconds by default)
     */
    ApicTimer(Util::Time::Timestamp timerInterval, Util::Time::Timestamp yieldInterval);

    /**
     * Copy Constructor.
     */
    ApicTimer(const ApicTimer &other) = delete;

    /**
     * Assignment operator.
     */
    ApicTimer &operator=(const ApicTimer &other) = delete;

    /**
     * Destructor.
     */
    ~ApicTimer() override = default;

    /**
     * Overriding function from InterruptHandler.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] Util::Time::Timestamp getTime() override;

    /**
     * Calibrate the APIC timer using the PIT.
     *
     * Uses the PIT to measure how often the APIC timer ticks in 10ms.
     * When constructing a new timer, this value will be used to calculate the initial counter for the desired interval.
     */
    static void calibrate();

    [[nodiscard]] uint8_t getCpuId() const;

private:
    uint8_t cpuId;          // The id of the CPU that uses this timer.
    Util::Time::Timestamp timerInterval; // The interrupt trigger interval in milliseconds.
    Util::Time::Timestamp yieldInterval; // The preemption trigger interval in milliseconds.
    Util::Time::Timestamp timeSinceLastYield;

    Util::Time::Timestamp time{}; // The "core-local" timestamp.

    static uint32_t BASE_FREQUENCY; // The number of ticks the APIC timer does in 1 second
};

}

#endif
