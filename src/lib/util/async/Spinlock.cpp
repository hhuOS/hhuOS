#include "Spinlock.h"

namespace Util::Async {

Spinlock::Spinlock() : lockVarWrapper(lockVar) {}

void Spinlock::acquire() {
    while (!tryAcquire()) {}
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