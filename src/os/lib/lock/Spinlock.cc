#include <kernel/cpu/Cpu.h>
#include "Spinlock.h"

extern "C" {
    void acquireLock(uint32_t* ptr);
    void releaseLock(uint32_t* ptr);
    uint32_t tryAcquireLock(uint32_t* ptr);
}

void Spinlock::acquire() {

	acquireLock(&lockVar);
}

bool Spinlock::tryLock() {

    return tryAcquireLock(&lockVar) == ZERO_FLAG;
}

void Spinlock::release() {

    releaseLock(&lockVar);
}

Spinlock::Spinlock() noexcept : lockVar(SPINLOCK_UNLOCK) {

}

bool Spinlock::isLocked() {

    return lockVar == SPINLOCK_LOCK;
}


