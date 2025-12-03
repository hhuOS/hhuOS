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

#ifndef HHUOS_LIB_UTIL_ASYNC_SPINLOCK_H
#define HHUOS_LIB_UTIL_ASYNC_SPINLOCK_H

#include <stdint.h>

#include "util/async/Atomic.h"
#include "util/async/Lock.h"
#include "util/async/Thread.h"

namespace Util {
namespace Async {

/// A simple spinlock implementation.
/// It is implemented using Util::Async::Atomic on an 8-bit integer.
///
/// ## Example
///
/// ```c++
/// int value = 0; // Global variable
/// Spinlock lock; // Lock to synchronize access to the global variable
///
/// // Function that runs in a thread.
/// // The spinlock is used to synchronize access to the global variable.
/// // The final value should be 1000 times the amount of threads executing this function.
/// // If we would not use the lock, the final value would be undefined due to the `Lost Update` problem.
/// void threadFunction() {
///     for (size_t i = 0; i < 1000; i++) {
///         lock.acquire();
///         const auto oldValue = value++;
///         lock.release();
///
///         Util::System::out << "Incremented value from " << oldValue << " to " << (oldValue + 1)
///             << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     }
/// }
/// ```
class Spinlock : public Lock {

public:
    /// Create a new spinlock.
    Spinlock() = default;

    /// Acquire the lock.
    /// This function calls `tryAcquire()` in a loop until the lock is acquired.
    /// Every time `tryAcquire()` fails, `Thread::yield()` is called to allow other threads to run.
    void acquire() final {
        while (!tryAcquire()) {
            Thread::yield();
        }
    }

    /// Try to acquire the lock once.
    /// If the lock is not available, the function does not block and returns false.
    bool tryAcquire() override {
        return lockVarWrapper.compareAndSet(SPINLOCK_UNLOCK, SPINLOCK_LOCK);
    }

    /// Release the lock.
    /// If the lock is not held, this function does nothing.
    void release() override {
        lockVarWrapper.set(SPINLOCK_UNLOCK);
    }

    /// Check if the lock is currently held.
    bool isLocked() const final {
        return lockVarWrapper.get() != SPINLOCK_UNLOCK;
    }

protected:

    /// Grant access to the lock variable for subclasses.
    Atomic<uint16_t> lockVarWrapper = Atomic<uint16_t>(lockVar);

    /// The unlocked state is represented by the value `UINT16_MAX`.
    static constexpr uint16_t SPINLOCK_UNLOCK = UINT16_MAX;

private:

    uint16_t lockVar = SPINLOCK_UNLOCK;

    static constexpr uint16_t SPINLOCK_LOCK = 0x01;
};

}
}

#endif

