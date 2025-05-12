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

#include "ReentrantSpinlock.h"

#include "lib/interface.h"
#include "async/Atomic.h"
#include "async/Thread.h"

namespace Util::Async {

bool ReentrantSpinlock::tryAcquire() {
    const auto threadId = isSchedulerInitialized() ? Thread::getCurrentThread().getId() : 0;
    const auto success = lockVarWrapper.compareAndSet(SPINLOCK_UNLOCK, threadId) ||
        lockVarWrapper.compareAndSet(threadId, threadId);

    if (success) {
        depth++;
    }

    return success;
}

void ReentrantSpinlock::release() {
    const auto threadId = isSchedulerInitialized() ? Thread::getCurrentThread().getId() : 0;

    if (lockVarWrapper.get() == threadId) {
        depth--;
    }

    if (depth == 0) {
        lockVarWrapper.compareAndSet(threadId, SPINLOCK_UNLOCK);
    }
}

size_t ReentrantSpinlock::getDepth() const {
    return depth;
}

}