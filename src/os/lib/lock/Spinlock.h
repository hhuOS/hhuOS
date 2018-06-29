/*
* Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
* Heinrich-Heine University
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
#include "Lock.h"

/**
 * A simple spinlock implemented using test&set instructions.
 *
 * @author Michael Schoettner, Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski
 * @date HHU, 2018
 */
class Spinlock : public Lock {

public:

    Spinlock() noexcept;

    Spinlock(const Spinlock &other) = delete;

    Spinlock &operator=(const Spinlock &other) = delete;

    ~Spinlock() = default;

    /**
     * Try to get the lock and block until lock is acquired
     */
    void acquire() override;
    
    /**
	 * Unlock the lock.
	 */
    void release() override;

    bool isLocked() override;

    /**
     * Try to acquire the lock once and return if it was successful.
     *
     * @return true = successful, false = not successful
     */
    bool tryLock();

private:

    /**
     * The value which is used for locking
     */
    uint32_t lockVar;

    /**
     * Position of the zero flag in EFLAG
     */
    const static uint32_t ZERO_FLAG = 0x40;

    static const uint32_t SPINLOCK_LOCK = 0x1;

    static const uint32_t SPINLOCK_UNLOCK = 0x0;
};

#endif

