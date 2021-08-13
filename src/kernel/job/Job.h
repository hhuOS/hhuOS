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

#ifndef HHUOS_JOB_H
#define HHUOS_JOB_H

#include <cstdint>
#include <lib/util/async/Runnable.h>
#include <lib/util/async/Spinlock.h>

namespace Kernel {

class Job {

public:

    Job() = default;

    Job(Util::Async::Runnable &runnable, int64_t interval, int32_t repetitions = -1);

    /**
     * Copy constructor.
     */
    Job(const Job &other) = default;

    /**
     * Assignment operator.
     */
    Job &operator=(const Job &other) = default;

    /**
     * Destructor.
     */
    ~Job() = default;

    void advanceTime(int64_t elapsedTime);

    [[nodiscard]] uint32_t getId() const;

    [[nodiscard]] bool isFinished() const;

    bool operator==(const Job &other) const;

    bool operator!=(const Job &other) const;

private:

    [[nodiscard]] static uint32_t generateId();

    Util::Async::Runnable *runnable = nullptr;

    uint32_t id = 0;
    int64_t interval = 0;
    int64_t currentTime = 0;
    int32_t repetitionsLeft = 0;

};

}

#endif