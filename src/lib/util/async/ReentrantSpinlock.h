/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_LIB_UTIL_ASYNC_REENTRANTSPINLOCK_H
#define HHUOS_LIB_UTIL_ASYNC_REENTRANTSPINLOCK_H

#include <stddef.h>

#include "async/Spinlock.h"

namespace Util::Async {

/// Variation of the Spinlock class that allows reentrant locking.
/// If a thread tries to acquire a lock that it already holds, it will not block.
/// This is useful for recursive functions or when a thread needs to acquire a lock multiple times.
///
/// ## Example
///
/// ```c++
/// int value = 0; // Global variable
/// ReentrantSpinlock lock; // Lock to synchronize access to the global variable
///
/// // This function increments the global variable and calls checkEven() to check if the value is even.
/// // The function checkEven() also acquires the lock, which would result in a deadlock using a normal spinlock.
/// // However, using a reentrant spinlock allows the same thread to acquire the lock multiple times.
/// void incrementValue() {
///     lock.acquire();
///
///     value++;
///     checkEven(); // Call a function that also acquires the lock
///
///     lock.release();
/// }
///
/// void checkEven() {
///     lock.acquire();
///
///     if (value % 2 == 0) {
///         printf("Value is even: %u\n", value);
///     } else {
///         printf("Value is odd: %u\n",value);
///     }
///
///     lock.release();
/// }
/// ```
class ReentrantSpinlock final : public Spinlock {

public:
    /// Create a new reentrant spinlock.
    ReentrantSpinlock() = default;

    /// Try to acquire the lock once.
    /// If the lock is not available, the function does not block and returns false.
    [[nodiscard]] bool tryAcquire() override;

    /// Release the lock.
    /// If the lock is not held, this function does nothing.
    void release() override;

    /// Get the amount of times the lock has been acquired by the current thread.
    [[nodiscard]] size_t getDepth() const;

private:

    size_t depth = 0;
};

}

#endif
