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

#ifndef HHUOS_PIT_H
#define HHUOS_PIT_H

#include <cstdint>

#include "kernel/interrupt/InterruptHandler.h"
#include "device/cpu/IoPort.h"
#include "TimeProvider.h"
#include "lib/util/time/Timestamp.h"

namespace Kernel {
class Logger;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

class Pit : public Kernel::InterruptHandler, public TimeProvider {

public:
    /**
     * Constructor.
     *
     * @param timerInterval The interval, at which the PIT shall trigger interrupts.
     */
    explicit Pit(uint16_t interruptRateDivisor = 1193, uint32_t yieldInterval = 10);

    /**
     * Copy Constructor.
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

    static const constexpr uint32_t BASE_FREQUENCY = 1193182;

private:

    /**
     * Sets the interval at which the PIT fires interrupts.
     *
     * @param ns The interval in nanoseconds
     */
    void setInterruptRate(uint16_t divisor);

    Util::Time::Timestamp time{};
    uint32_t timerInterval = 0;
    uint32_t yieldInterval;

    IoPort controlPort = IoPort(0x43);
    IoPort dataPort0 = IoPort(0x40);

    static Kernel::Logger log;

};

}

#endif
