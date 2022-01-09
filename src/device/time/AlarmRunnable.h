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

#ifndef HHUOS_ALARMRUNNABLE_H
#define HHUOS_ALARMRUNNABLE_H

#include "lib/util/async/Runnable.h"

namespace Device {

class AlarmRunnable : public Util::Async::Runnable {

public:
    /**
     * Default Constructor.
     */
    AlarmRunnable() = default;

    /**
     * Copy constructor.
     */
    AlarmRunnable(const AlarmRunnable &other) = delete;

    /**
     * Assignment operator.
     */
    AlarmRunnable &operator=(const AlarmRunnable &other) = delete;

    /**
     * Destructor.
     */
    ~AlarmRunnable() override = default;

    void run() override;

private:

    bool speakerOn = false;

};

}

#endif