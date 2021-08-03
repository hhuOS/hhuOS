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

#include "Job.h"

namespace Kernel {

Job::Job(Util::Async::Runnable &runnable, int64_t interval) : runnable(&runnable), id(generateId()), interval(interval), currentTime(interval) {}

void Job::advanceTime(int64_t elapsedTime) {
    currentTime -= elapsedTime;
    if (currentTime <= 0) {
        currentTime = interval + currentTime;
        runnable->run();
    }
}

uint32_t Job::generateId() {
    static uint32_t idCounter = 0;

    auto idWrapper = Util::Async::Atomic<uint32_t>(idCounter);
    return idWrapper.fetchAndInc();
}

uint32_t Job::getId() const {
    return id;
}

bool Job::operator==(const Job &other) const {
    return id == other.id;
}

bool Job::operator!=(const Job &other) const {
    return id != other.id;
}

}