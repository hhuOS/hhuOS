#include "Spinlock.h"

Spinlock::Spinlock() noexcept : lockVar(SPINLOCK_UNLOCK) {

}

void Spinlock::acquire() {
	while(lockVar.getAndSet(SPINLOCK_LOCK) !=  SPINLOCK_LOCK);
}

bool Spinlock::tryAcquire() {
    return lockVar.getAndSet(SPINLOCK_LOCK) ==  SPINLOCK_LOCK;
}

void Spinlock::release() {
    lockVar.set(SPINLOCK_UNLOCK);
}

bool Spinlock::isLocked() {

    return lockVar.get() == SPINLOCK_LOCK;
}


