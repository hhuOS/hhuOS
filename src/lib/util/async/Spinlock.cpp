#include "Spinlock.h"

namespace Util::Async {

Spinlock::Spinlock() noexcept: lockVarWrapper(lockVar) {

}

void Async::Spinlock::acquire() {
    while (lockVarWrapper.getAndSet(SPINLOCK_LOCK) != SPINLOCK_UNLOCK);
}

bool Spinlock::tryAcquire() {
    return lockVarWrapper.getAndSet(SPINLOCK_LOCK) == SPINLOCK_UNLOCK;
}

void Spinlock::release() {
    lockVarWrapper.set(SPINLOCK_UNLOCK);
}

bool Spinlock::isLocked() {
    return lockVarWrapper.get() == SPINLOCK_LOCK;
}

}