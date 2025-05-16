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

#ifndef HHUOS_LIB_UTIL_ASYNC_THREAD_H
#define HHUOS_LIB_UTIL_ASYNC_THREAD_H

#include <stddef.h>

#include "base/String.h"

namespace Util {
namespace Async {
class Runnable;
}  // namespace Async
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Util::Async {

/// Create and manipulate threads from the user space.
/// This class just wraps a thread ID and uses systems calls to manipulate the thread referenced by the ID.
class Thread {

public:
    /// Create an instance with the given ID.
    /// This constructor does not create a new thread, it just wraps the given ID.
    explicit Thread(size_t id);

    /// Start a new thread with the given name and runnable.
    /// The runnable must be heap allocated and will be deleted by the thread when it is done.
    static Thread createThread(const String &name, Runnable *runnable);

    /// Get access to the current thread.
    ///
    /// ### Example
    ///
    /// ```c++
    /// auto thread = Util::Async::Thread::getCurrentThread();
    /// printf("Current thread ID: %u\n", thread.getId());
    /// ```
    static Thread getCurrentThread();

    /// Let the current thread sleep for the given time.
    ///
    /// ### Example
    ///
    /// ```c++
    /// Util::Async::Thread::sleep(Util::Time::Timestamp::ofMilliseconds(100)); // Sleep for 100 ms
    /// ```
    static void sleep(const Time::Timestamp &time);

    /// Switch to another thread.
    ///
    /// ### Example
    ///
    /// ```c++
    /// Util::Async::Thread::yield();
    /// ```
    static void yield();

    /// Join the thread by blocking until it has finished.
    ///
    /// ### Example
    ///
    /// ```c++
    /// // A basic runnable that prints a message.
    /// auto runnable = new Util::Async::BasicRunnable([]() {
    ///     printf("Hello from a new thread!\n");
    /// });
    ///
    /// auto thread = Util::Async::Thread::createThread("Test", runnable);
    ///
    /// thread.join(); // Wait for the thread to finish
    /// printf("Thread 'Test' has finished!");
    /// ```
    void join() const;

    /// Get the ID of the thread.
    [[nodiscard]] size_t getId() const;

private:

    const size_t id;
};

}

#endif
