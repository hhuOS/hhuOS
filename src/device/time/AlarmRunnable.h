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

#ifndef HHUOS_ALARMRUNNABLE_H
#define HHUOS_ALARMRUNNABLE_H

#include <cstdint>
#include "lib/util/async/Runnable.h"

namespace Device {

class AlarmRunnable : public Util::Async::Runnable {

public:
    /**
     * Constructor.
     */
    explicit AlarmRunnable(uint32_t beepCount = 3);

    /**
     * Copy Constructor.
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

    uint32_t beepCount;
    bool speakerOn = false;

    static const constexpr uint32_t INTERVAL = 500;
};

}

#endif