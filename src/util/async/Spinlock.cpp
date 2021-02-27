#include "device/cpu/Cpu.h"
#include "Spinlock.h"

namespace Util::Async {

Spinlock::Spinlock() noexcept: lockVar(SPINLOCK_UNLOCK) {

}

void Async::Spinlock::acquire() {
    while (lockVar.getAndSet(SPINLOCK_LOCK) != SPINLOCK_UNLOCK);
}

bool Spinlock::tryAcquire() {
    return lockVar.getAndSet(SPINLOCK_LOCK) == SPINLOCK_UNLOCK;
}

void Spinlock::release() {
    lockVar.set(SPINLOCK_UNLOCK);
}

bool Spinlock::isLocked() {
    return lockVar.get() == SPINLOCK_LOCK;
}

}