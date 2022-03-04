/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "lib/util/async/Runnable.h"
#include "lib/util/async/Spinlock.h"
#include "lib/util/time/Timestamp.h"

namespace Kernel {

class Job {

public:

    enum Priority {
        LOW,
        HIGH
    };

    struct Id {
        uint32_t value;

        bool operator==(const Id &other) const;

        bool operator!=(const Id &other) const;
    };

    Job() = default;

    Job(Util::Async::Runnable *runnable, Util::Time::Timestamp interval, int32_t repetitions = -1);

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
    ~Job();

    void advanceTime(Util::Time::Timestamp elapsedTime);

    void executeIfPending();

    [[nodiscard]] Id getId() const;

    [[nodiscard]] bool isFinished() const;

    bool operator==(const Job &other) const;

    bool operator!=(const Job &other) const;

private:

    [[nodiscard]] static Id generateId();

    Util::Async::Runnable *runnable = nullptr;

    Id id{};
    int64_t interval = 0;
    int64_t currentTime = 0;
    int32_t repetitionsLeft = 0;

};

}

#endif