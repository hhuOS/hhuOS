/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#include "Thread.h"

#include "lib/interface.h"

namespace Util {
class String;

namespace Async {
class Runnable;
}  // namespace Async
namespace Time {
class Timestamp;
}  // namespace Time
}  // namespace Util

namespace Util::Async {

Thread::Thread(const size_t id) : id(id) {}

Thread Thread::createThread(const String &name, Runnable *runnable) {
    return ::createThread(name, runnable);
}

Thread Thread::getCurrentThread() {
    return ::getCurrentThread();
}

void Thread::sleep(const Time::Timestamp &time) {
    ::sleep(time);
}

void Thread::yield() {
    ::yield();
}

void Thread::join() const {
    joinThread(id);
}

size_t Thread::getId() const {
    return id;
}

}