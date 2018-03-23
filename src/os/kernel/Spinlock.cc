#include "kernel/Spinlock.h"

extern "C" {
    void acquireLock(uint32_t* ptr);
    void releaseLock(uint32_t* ptr);
    uint32_t tryAcquireLock(uint32_t* ptr);
}

void Spinlock::lock() {
	acquireLock(ptr);
}

bool Spinlock::tryLock() {
    return tryAcquireLock(ptr) == ZERO_FLAG;
}

void Spinlock::unlock() {
    releaseLock(ptr);
}

Spinlock::Spinlock() noexcept {
    lock_var = 0;
    ptr  = &lock_var;
}


