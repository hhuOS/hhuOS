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

#ifndef __Spinlock_include__
#define __Spinlock_include__

#include <cstdint>

#include "lib/util/async/Atomic.h"
#include "Lock.h"

namespace Util::Async {

/**
 * A simple spinlock implemented using test&set instructions.
 *
 * @author Michael Schoettner, Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski
 * @date HHU, 2018
 */
class Spinlock : public Lock {

public:

    Spinlock();

    Spinlock(const Spinlock &other) = delete;

    Spinlock &operator=(const Spinlock &other) = delete;

    ~Spinlock() override = default;

    void acquire() override;

    bool tryAcquire() override;

    void release() override;

    bool isLocked() override;

protected:

    uint32_t lockVar = SPINLOCK_UNLOCK;
    Atomic<uint32_t> lockVarWrapper;

    static const constexpr uint32_t SPINLOCK_UNLOCK = UINT32_MAX;
    static const constexpr uint32_t SPINLOCK_LOCK = 0x01;
};

}

#endif

