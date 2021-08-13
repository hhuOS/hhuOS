/*
 * Copyright (C) 2018-2021 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_PIT_H
#define HHUOS_PIT_H

#include <kernel/interrupt/InterruptHandler.h>
#include <device/cpu/IoPort.h>
#include <kernel/job/JobExecutor.h>
#include "TimeProvider.h"

namespace Device {

class Pit : public Kernel::InterruptHandler, public Kernel::JobExecutor, public TimeProvider {

public:

    /**
     * Copy constructor.
     */
    Pit(const Pit &other) = delete;

    /**
     * Assignment operator.
     */
    Pit &operator=(const Pit &other) = delete;

    /**
     * Destructor.
     */
    ~Pit() override = default;

    /**
     * Get the singleton instance of the PIT.
     *
     * @return The instance of the PIT
     */
    [[nodiscard]] static Pit& getInstance();

    /**
     * Overriding function from InterruptHandler.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(Kernel::InterruptFrame &frame) override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] uint32_t getNanos() override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] uint32_t getMicros() override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] uint32_t getMillis() override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] uint32_t getSeconds() override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] uint32_t getMinutes() override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] uint32_t getHours() override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] uint32_t getDays() override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] uint32_t getYears() override;

private:

    /**
     * Constructor.
     *
     * @param timerInterval The interval, at which the PIT shall trigger interrupts.
     */
    explicit Pit(uint32_t timerInterval = DEFAULT_TIMER_INTERVAL);

    /**
     * Sets the interval at which the PIT fires interrupts.
     *
     * @param ns The interval in nanoseconds
     */
    void setInterval(uint32_t ns);

    static Pit *instance;

    Time time;
    uint32_t timerInterval;

    IoPort controlPort = IoPort(0x43);
    IoPort dataPort0 = IoPort(0x40);

    static const constexpr uint32_t TIME_BASE = 838;
    static const constexpr uint32_t DEFAULT_TIMER_INTERVAL = 1000000;

};

}

#endif