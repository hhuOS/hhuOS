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

#ifndef HHUOS_DATEPROVIDER_H
#define HHUOS_DATEPROVIDER_H

#include <cstdint>

namespace Device {

class DateProvider {

public:
    /**
     * Contains Date- and Time-information.
     */
    struct Date {
        uint8_t seconds = 0;
        uint8_t minutes = 0;
        uint8_t hours = 0;
        uint8_t dayOfMonth = 0;
        uint8_t month = 0;
        uint16_t year = 0;

        bool operator== (Date &date) const {
            return seconds == date.seconds && minutes == date.minutes && hours == date.hours &&
            dayOfMonth == date.dayOfMonth && month == date.month && year == date.year;
        }

        bool operator!= (Date &date) const {
            return seconds != date.seconds || minutes != date.minutes || hours != date.hours ||
            dayOfMonth != date.dayOfMonth || month != date.month || year != date.year;
        }
    };

    /**
     * Default Constructor.
     */
    DateProvider() = default;

    /**
     * Copy constructor.
     */
    DateProvider(const DateProvider &other) = delete;

    /**
     * Assignment operator.
     */
    DateProvider &operator=(const DateProvider &other) = delete;

    /**
     * Destructor.
     */
    ~DateProvider() = default;

    /**
     * Get the current date.
     */
    [[nodiscard]] virtual Date getCurrentDate() = 0;

};

}

#endif