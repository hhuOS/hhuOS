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

#ifndef HHUOS_PIT_H
#define HHUOS_PIT_H

#include <cstdint>

#include "kernel/interrupt/InterruptHandler.h"
#include "device/cpu/IoPort.h"
#include "device/time/TimeProvider.h"
#include "lib/util/time/Timestamp.h"
#include "lib/util/async/Atomic.h"
#include "lib/util/async/Spinlock.h"
#include "device/time/WaitTimer.h"

namespace Kernel {
enum InterruptVector : uint8_t;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

class Pit : public Kernel::InterruptHandler, public TimeProvider, public WaitTimer {

public:
    /**
     * Constructor with interval rate.
     * Configures the PIT to generate periodic interrupts at a given rate and drive the scheduler
     *
     * @param timerInterval The interval at which the PIT shall trigger interrupts.
     * @param yieldInterval The interval at which the scheduler shall be yielded.
     */
    explicit Pit(const Util::Time::Timestamp &timerInterval, const Util::Time::Timestamp &yieldInterval);

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
    void trigger(const Kernel::InterruptFrame &frame, Kernel::InterruptVector slot) override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] Util::Time::Timestamp getTime() override;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] bool isLocked() const override;

    /**
     * Wait for a specified amount of time.
     *
     * Intended for usage before interrupts are enabled and the PIT has been initialized to
     * rate-generator mode. It may not be used afterwards!
     *
     * @param ms The delay in microseconds
     */
    void wait(const Util::Time::Timestamp &waitTime) override;

    static const constexpr uint32_t BASE_FREQUENCY = 1193182;

private:

    enum BcdBinaryMode : uint8_t {
        BINARY = 0x00,
        BCD = 0x01
    };

    enum OperatingMode : uint8_t {
        INTERRUPT_ON_TERMINAL_COUNT = 0x00,
        ONE_SHOT = 0x01,
        RATE_GENERATOR = 0x02,
        SQUARE_WAVE_GENERATOR = 0x03,
        SOFTWARE_TRIGGERED_STROBE = 0x04,
        HARDWARE_TRIGGERED_STROBE = 0x05,
    };

    enum AccessMode : uint8_t {
        LATCH_COUNT = 0x00,
        LOW_BYTE_ONLY = 0x01,
        HIGH_BYTE_ONLY = 0x02,
        LOW_BYTE_HIGH_BYTE = 0x03
    };

    enum Channel : uint8_t  {
        CHANNEL_0 = 0x00,
        CHANNEL_1 = 0x01,
        CHANNEL_2 = 0x02,
        READ_BACK = 0x03
    };

    struct Command {
        Command(OperatingMode operatingMode, AccessMode accessMode);

        explicit operator uint8_t() const;

    private:

        BcdBinaryMode bcdBinaryMode: 1;
        OperatingMode operatingMode: 3;
        AccessMode accessMode: 2;
        uint8_t channel: 2;
    } __attribute__ ((packed));

    uint16_t readTimer();

    /**
     * Sets the interval at which the PIT fires interrupts.
     *
     * @param interval The interval
     */
    void setInterruptRate(const Util::Time::Timestamp &interval);

    void setDivisor(uint16_t divisor);

    bool yield = false;
    Util::Time::Timestamp time;
    Util::Time::Timestamp timeSinceLastYield;
    Util::Time::Timestamp timerInterval;
    Util::Time::Timestamp yieldInterval;

    uint32_t intervals = 0;
    uint32_t readerCount = 0;

    IoPort controlPort = IoPort(0x43);
    IoPort dataPort0 = IoPort(0x40);

    Util::Async::Spinlock readTimerLock;

    static const constexpr uint32_t NANOSECONDS_PER_TICK = 1000000000 / 1193182;
};

}

#endif
