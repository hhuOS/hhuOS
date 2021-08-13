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

#ifndef HHUOS_RTC_H
#define HHUOS_RTC_H

#include <kernel/interrupt/InterruptHandler.h>
#include "DateProvider.h"

namespace Device {

/**
 * Driver for the CMOS Realtime clock.
 */
class Rtc : public DateProvider, Kernel::InterruptHandler {

public:
    /**
     * Constructor.
     */
    Rtc();

    /**
     * Copy-constructor.
     */
    Rtc(const Rtc &copy) = delete;

    /**
     * Destructor.
     */
    ~Rtc() override = default;

    /**
     * Get the singleton instance of the RTC.
     *
     * @return The instance of the RTC
     */
    [[nodiscard]] static Rtc& getInstance();

    /**
     * Enable periodic interrupts for the RTC.
     */
    void plugin() override;

    /**
     * Overriding function from InterruptHandler.
     */
    void trigger(Kernel::InterruptFrame &frame) override;

    /**
     * Get the current date.
     */
    [[nodiscard]] Date getCurrentDate() override;

    /**
     * Set the RTC's date.
     */
    void setHardwareDate(const Date &date);

private:
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

    [[nodiscard]] Date readDate() const;

    /**
     * RTC-Registers.
     */
    enum REGISTERS {
        SECONDS_REGISTER = 0x00,
        MINUTES_REGISTER = 0x02,
        HOURS_REGISTER = 0x04,
        DAY_OF_WEEK_REGISTER = 0x06,
        DAY_OF_MONTH_REGISTER = 0x07,
        MONTH_REGISTER = 0x08,
        YEAR_REGISTER = 0x09,
        CENTURY_REGISTER = 0x32,
        STATUS_REGISTER_A = 0x0A,
        STATUS_REGISTER_B = 0x0B,
        STATUS_REGISTER_C = 0x0C
    };

    Rtc::Date currentDate {};
    bool useBcd;
    bool useTwelveHours;

    static const constexpr uint8_t CURRENT_CENTURY = 20;
    static const constexpr uint8_t RTC_RATE = 0x06;

};

}

#endif