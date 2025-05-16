/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#include "lib/util/time/Date.h"

namespace Device {

class DateProvider {

public:
    /**
     * Default Constructor.
     */
    DateProvider() = default;

    /**
     * Copy Constructor.
     */
    DateProvider(const DateProvider &other) = delete;

    /**
     * Assignment operator.
     */
    DateProvider &operator=(const DateProvider &other) = delete;

    /**
     * Destructor.
     */
    virtual ~DateProvider() = default;

    /**
     * Get the current date.
     */
    [[nodiscard]] virtual Util::Time::Date getCurrentDate() = 0;

    /**
     * Set the current date.
     */
    virtual void setCurrentDate(const Util::Time::Date &date) = 0;

};

}

#endif