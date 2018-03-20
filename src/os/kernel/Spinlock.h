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

/**
 * Spinlock - a simple spinlock implemented using test&set instructions
 *
 * @author Michael Schoettner, Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski
 * @date HHU, 2018
 */
class Spinlock {
    
private:
	// the value which is used for locking
    unsigned int lock_var;
    // pointer to the lock-value
    unsigned int *ptr;

public:

    Spinlock(const Spinlock &other) = delete;
    /**
     * Constructor - initializes Spinlock.
     */
    Spinlock() noexcept {
        lock_var = 0;
        ptr  = &lock_var;
    }

    ~Spinlock() = default;

    /**
     * Try to get the lock and block until lock is acquired
     */
    void lock();
    
    /**
	 * Unlock the lock.
	 */
    void unlock();

    /**
     * Try to acquire the lock once and return if it was successful.
     *
     * @return true = successful, false = not successful
     */
    bool tryLock();

    // Position of the zero flag in EFLAG
    const static uint32_t ZERO_FLAG = 0x40;
};

#endif

