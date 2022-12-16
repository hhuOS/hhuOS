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

#ifndef HHUOS_REENTRANTSPINLOCK_H
#define HHUOS_REENTRANTSPINLOCK_H

#include <cstdint>

#include "Spinlock.h"

namespace Util::Async {

class ReentrantSpinlock : public Spinlock {

public:
    /**
     * Default Constructor.
     */
    ReentrantSpinlock() = default;

    /**
     * Copy Constructor.
     */
    ReentrantSpinlock(const ReentrantSpinlock &other) = delete;

    /**
     * Assignment operator.
     */
    ReentrantSpinlock &operator=(const ReentrantSpinlock &other) = delete;

    /**
     * Destructor.
     */
    ~ReentrantSpinlock() override = default;

    bool tryAcquire() override;

    void release() override;

    [[nodiscard]] uint32_t getDepth() const;

private:

    uint32_t depth = 0;
};

}

#endif
