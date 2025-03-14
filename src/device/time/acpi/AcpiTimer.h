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

#ifndef HHUOS_ACPITIMER_H
#define HHUOS_ACPITIMER_H

#include <stdint.h>

#include "device/cpu/IoPort.h"
#include "device/time/WaitTimer.h"

namespace Device {

class AcpiTimer : public WaitTimer {

public:
    /**
     * Constructor.
     */
    AcpiTimer();

    /**
     * Copy Constructor.
     */
    AcpiTimer(const AcpiTimer &other) = delete;

    /**
     * Assignment operator.
     */
    AcpiTimer &operator=(const AcpiTimer &other) = delete;

    /**
     * Destructor.
     */
    ~AcpiTimer() override = default;

    static bool isAvailable();

    uint32_t readTimer();

    void wait(const Util::Time::Timestamp &waitTime) override;

private:

    IoPort timerPort = IoPort(0);

    uint32_t maxValue = UINT32_MAX;

    static const constexpr uint32_t FREQUENCY = 3579545;
    static const constexpr uint32_t NANOSECONDS_PER_TICK = 1000000000 / FREQUENCY;
};

}

#endif
