/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_WAITTIMER_H
#define HHUOS_WAITTIMER_H

#include "lib/util/time/Timestamp.h"

namespace Device {

class WaitTimer {

public:
    /**
     * Default Constructor.
     */
    WaitTimer() = default;

    /**
     * Copy Constructor.
     */
    WaitTimer(const WaitTimer &other) = delete;

    /**
     * Assignment operator.
     */
    WaitTimer &operator=(const WaitTimer &other) = delete;

    /**
     * Destructor.
     */
    virtual ~WaitTimer() = default;

    virtual void wait(const Util::Time::Timestamp &waitTime) = 0;
};

}

#endif
