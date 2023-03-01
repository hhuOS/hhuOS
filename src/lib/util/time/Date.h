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
 */

#ifndef HHUOS_DATE_H
#define HHUOS_DATE_H

#include <cstdint>

namespace Util::Time {

/**
 * Represents a date and time.
 */
class Date {

public:
    /**
     * Default Constructor.
     */
    Date() = default;

    Date(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t dayOfMonth, uint8_t month, uint16_t year);

    /**
     * Copy Constructor.
     */
    Date(const Date &copy) = default;

    /**
     * Assignment operator.
     */
    Date &operator=(const Date &other) = default;

    /**
     * Destructor.
     */
    ~Date() = default;

    bool operator==(Date &date) const {
        return seconds == date.seconds && minutes == date.minutes && hours == date.hours &&
               dayOfMonth == date.dayOfMonth && month == date.month && year == date.year;
    }

    bool operator!=(Date &date) const {
        return seconds != date.seconds || minutes != date.minutes || hours != date.hours ||
               dayOfMonth != date.dayOfMonth || month != date.month || year != date.year;
    }

    [[nodiscard]] uint8_t getSeconds() const;

    void setSeconds(uint8_t seconds);

    [[nodiscard]] uint8_t getMinutes() const;

    void setMinutes(uint8_t minutes);

    [[nodiscard]] uint8_t getHours() const;

    void setHours(uint8_t hours);

    [[nodiscard]] uint8_t getDayOfMonth() const;

    void setDayOfMonth(uint8_t dayOfMonth);

    [[nodiscard]] uint8_t getMonth() const;

    void setMonth(uint8_t month);

    [[nodiscard]] uint16_t getYear() const;

    void setYear(uint16_t year);

private:

    uint8_t seconds = 0;
    uint8_t minutes = 0;
    uint8_t hours = 0;
    uint8_t dayOfMonth = 0;
    uint8_t month = 0;
    uint16_t year = 0;

};

Date getCurrentDate();

void setDate(const Date &date);

}

#endif