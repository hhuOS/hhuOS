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

#include "lib/interface.h"
#include "lib/util/async/Thread.h"

namespace Util {
namespace Async {
class Runnable;
}  // namespace Async
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Util::Async {

Thread::Thread(uint32_t id) : id(id) {}

Thread Util::Async::Thread::getCurrentThread() {
    return ::getCurrentThread();
}

uint32_t Thread::getId() const {
    return id;
}

void Thread::sleep(const Time::Timestamp &time) {
    ::sleep(time);
}

void Thread::yield() {
    ::yield();
}

Thread Thread::createThread(const Util::Memory::String &name, Runnable *runnable) {
    return ::createThread(name, runnable);
}

void Thread::join() const {
    ::joinThread(id);
}

}