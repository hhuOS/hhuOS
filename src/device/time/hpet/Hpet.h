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

#ifndef HHUOS_HPET_H
#define HHUOS_HPET_H

#include "device/time/TimeProvider.h"
#include "device/time/WaitTimer.h"
#include "kernel/interrupt/InterruptFrame.h"
#include "kernel/interrupt/InterruptVector.h"
#include "kernel/interrupt/InterruptHandler.h"

namespace Device {

class Hpet : public WaitTimer {

public:

    enum Register : uint16_t {
        GENERAL_CAPABILITIES_ID = 0x00,
        GENERAL_CONFIGURATION = 0x10,
        GENERAL_INTERRUPT_STATUS = 0x20,
        MAIN_COUNTER_VALUE = 0xf0,
        TIMER_CONFIGURATION_AND_CAPABILITIES = 0x100,
        TIMER_COMPARATOR_VALUE = 0x108,
        TIMER_FSB_INTERRUPT_ROUTE = 0x110,
    };

    /**
     * Default Constructor.
     */
    Hpet();

    /**
     * Copy Constructor.
     */
    Hpet(const Hpet &other) = delete;

    /**
     * Assignment operator.
     */
    Hpet &operator=(const Hpet &other) = delete;

    /**
     * Destructor.
     */
    ~Hpet() = default;

    static bool isAvailable();

    uint64_t readRegister(uint16_t offset);

    void writeRegister(uint16_t offset, uint64_t value);

    uint64_t readCounter();

    /**
     * Overriding function from WaitTimer.
     */
    void wait(const Util::Time::Timestamp &waitTime) override;

private:

    enum Configuration : uint64_t {
        ENABLE = 1 << 0,
        LEGACY_REPLACEMENT = 1 << 1,
    };

    uint8_t *baseAddress = nullptr;
    uint64_t counterClockPeriod = 0;
    uint64_t maxValue = 0;
};

}

#endif
