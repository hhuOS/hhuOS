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

#ifndef HHUOS_LIB_UTIL_ASYNC_FUNCTIONPOINTERRUNNABLE_H
#define HHUOS_LIB_UTIL_ASYNC_FUNCTIONPOINTERRUNNABLE_H

#include "Runnable.h"

namespace Util::Async {

/// A Runnable that wraps a function pointer.
/// The function does not take any arguments and does not return anything.
/// This is the most basic implementation of a Runnable and allows to run a function in a thread.
///
/// ## Example
///
/// ```c++
/// // Create a runnable with a lambda function.
/// // A Runnable needs to be heap allocated for using it with the Util::Async::Thread class.
/// // The Thread instance will take ownership of the runnable and delete it when the thread is done.
/// auto runnable = new Util::Async::BasicRunnable([]() {
///     printf("Hello from a new thread!\n");
/// });
///
/// auto thread = Util::Async::Thread::createThread("Test", runnable);
/// ```
class BasicRunnable final : public Runnable {

public:
    /// Create a new Runnable with the given function pointer.
    explicit BasicRunnable(void (*function)());

    /// Run the Runnable.
    /// This is called by a thread when it starts.
    void run() override;

private:

    void (*function)();
};

}

#endif