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

#ifndef HHUOS_RTC_H
#define HHUOS_RTC_H

#include <cstdint>

#include "kernel/interrupt/InterruptHandler.h"
#include "DateProvider.h"
#include "TimeProvider.h"
#include "lib/util/time/Date.h"
#include "lib/util/time/Timestamp.h"

namespace Kernel {
class Logger;
struct InterruptFrame;
}  // namespace Kernel

namespace Device {

/**
 * Driver for the CMOS Realtime clock.
 */
class Rtc : public DateProvider, public TimeProvider, public Kernel::InterruptHandler {

public:
    /**
     * Constructor.
     *
     * @param interruptRateDivisor Divisor, determining interrupt rate (14 -> 250ms)
     */
    explicit Rtc(uint8_t interruptRateDivisor = 14);

    /**
     * Copy Constructor.
     */
    Rtc(const Rtc &copy) = delete;

    /**
     * Assignment operator.
     */
    Rtc& operator=(const Rtc &other) = delete;

    /**
     * Destructor.
     */
    ~Rtc() override = default;

    /**
     * Check, if an RTC is installed in the system, by trying to set different rates for the periodic interrupt.
     *
     * @return true, if an RTC is available
     */
    [[nodiscard]] static bool isAvailable();

    /**
     * Enable periodic interrupts for the RTC.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(const Kernel::InterruptFrame &frame) override;

    /**
     * Check if CMOS data is valid or if power was lost.
     *
     * @return true, if CMOS data is valid
     */
    [[nodiscard]] static bool isValid();

    /**
     * Overriding function from DateProvider
     */
    [[nodiscard]] Util::Time::Date getCurrentDate() override;

    /**
     * Overriding function from DateProvider
     */
    void setCurrentDate(const Util::Time::Date &date) override;

    /**
     * Set an alarm at a specific time.
     *
     * @param date The time, at which the alarm should start (date is ignored)
     */
    void setAlarm(const Util::Time::Date &date) const;

    /**
     * Overriding function from TimeProvider.
     */
    [[nodiscard]] Util::Time::Timestamp getTime() override;

private:
    /**
     * RTC-Registers.
     */
    enum RtcRegister : uint8_t {
        SECONDS_REGISTER = 0x00,
        ALARM_SECONDS_REGISTER = 0x01,
        MINUTES_REGISTER = 0x02,
        ALARM_MINUTES_REGISTER = 0x03,
        HOURS_REGISTER = 0x04,
        ALARM_HOURS_REGISTER = 0x05,
        DAY_OF_WEEK_REGISTER = 0x06,
        DAY_OF_MONTH_REGISTER = 0x07,
        MONTH_REGISTER = 0x08,
        YEAR_REGISTER = 0x09,
        STATUS_REGISTER_A = 0x0A,
        STATUS_REGISTER_B = 0x0B,
        STATUS_REGISTER_C = 0x0C,
        STATUS_REGISTER_D = 0x0D
    };

    /**
     * Converts a number from bcd format to binary format.
     *
     * @param bcd The bcd formatted number
     * @return The converted number
     */
    [[nodiscard]] static uint8_t bcdToBinary(uint8_t bcd);

    /**
     * Converts a number from binary format to bcd format.
     *
     * @param binary The binary formatted number
     * @return The converted number
     */
    [[nodiscard]] static uint8_t binaryToBcd(uint8_t binary);

    /**
     * Checks, if the RTC is currently updating.
     * Read/Write operations should not be performed, while an update is in progress.
     */
    [[nodiscard]] static bool isUpdating();

    /**
     * Read the current date from the RTC.
     *
     * @return The date
     */
    [[nodiscard]] Util::Time::Date readDate() const;

    /**
     * Notify the user about an alarm.
     */
    void alarm();

    /**
     * Set the rate at which the RTC fires periodic interrupts.
     *
     * @param ns The interval in nanoseconds
     */
    void setInterruptRate(uint8_t divisor);

    Util::Time::Timestamp time{};
    Util::Time::Date currentDate{};
    uint32_t timerInterval = 0;

    uint8_t centuryRegister = 0;

    bool useBcd;
    bool useTwelveHours;

    static Kernel::Logger log;

    static const constexpr uint32_t BASE_FREQUENCY = 32768;
    static const constexpr uint8_t CURRENT_CENTURY = 20;

    static const constexpr uint8_t INTERRUPT_UPDATE_ENDED = 0x10;
    static const constexpr uint8_t INTERRUPT_ALARM = 0x20;
    static const constexpr uint8_t INTERRUPT_PERIODIC = 0x40;

};

}

#endif