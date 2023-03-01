/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
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
#include "Spinlock.h"

namespace Util::Async {

Spinlock::Spinlock() : lockVarWrapper(lockVar) {}

void Spinlock::acquire() {
    while (!tryAcquire()) {
        Thread::yield();
    }
}

bool Spinlock::tryAcquire() {
    return lockVarWrapper.compareAndSet(SPINLOCK_UNLOCK, SPINLOCK_LOCK);
}

void Spinlock::release() {
    lockVarWrapper.set(SPINLOCK_UNLOCK);
}

bool Spinlock::isLocked() {
    return lockVarWrapper.get() != SPINLOCK_UNLOCK;
}

}