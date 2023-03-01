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

#ifndef HHUOS_TIMEPROVIDER_H
#define HHUOS_TIMEPROVIDER_H

#include <cstdint>
#include "lib/util/time/Timestamp.h"

namespace Device {

class TimeProvider {

public:
    /**
     * Default Constructor.
     */
    TimeProvider() = default;

    /**
     * Copy Constructor.
     */
    TimeProvider(const TimeProvider &other) = delete;

    /**
     * Assignment operator.
     */
    TimeProvider &operator=(const TimeProvider &other) = delete;

    /**
     * Destructor.
     */
    virtual ~TimeProvider() = default;

    [[nodiscard]] virtual Util::Time::Timestamp getTime() = 0;

};

}

#endif