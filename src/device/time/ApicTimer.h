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

#ifndef HHUOS_APICTIMER_H
#define HHUOS_APICTIMER_H

#include "kernel/interrupt/InterruptHandler.h"
#include "TimeProvider.h"
#include "kernel/log/Logger.h"

namespace Device {

/**
 * This class implements the APIC timer device.
 *
 * Its purpose is to trigger per-core scheduler preemption in SMP systems, although it is also used
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
    ApicTimer(uint32_t timerInterval, uint32_t yieldInterval);

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
    void trigger(const Kernel::InterruptFrame &frame) override;

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
    uint32_t timerInterval; // The interrupt trigger interval in milliseconds.
    uint32_t yieldInterval; // The preemption trigger interval in milliseconds.

    Util::Time::Timestamp time{}; // The "core-local" timestamp.

    static uint32_t ticksPerMilliseconds; // The number of ticks the APIC timer does in 10 ms.
    static Divider divider;                // The used divider, it has to be consistent to get consistent timings.

    static Kernel::Logger log;
};

}

#endif
