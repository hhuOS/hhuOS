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

#ifndef HHUOS_UTIL_THREAD_H
#define HHUOS_UTIL_THREAD_H

#include <cstdint>

#include "lib/util/memory/String.h"

namespace Util {
namespace Async {
class Runnable;
}  // namespace Async
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Util::Async {

class Thread {

public:
    /**
     * Constructor.
     */
    explicit Thread(uint32_t id);

    /**
     * Copy Constructor.
     */
    Thread(const Thread &other) = delete;

    /**
     * Assignment operator.
     */
    Thread &operator=(const Thread &other) = delete;

    /**
     * Destructor.
     */
    ~Thread() = default;

    static Thread createThread(const Memory::String &name, Runnable *runnable);

    static Thread getCurrentThread();

    static void sleep(const Time::Timestamp &time);

    static void yield();

    [[nodiscard]] uint32_t getId() const;

    void join() const;

private:

    uint32_t id;
};

}

#endif
