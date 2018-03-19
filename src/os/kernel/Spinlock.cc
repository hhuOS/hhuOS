/**
 * Spinlock - a simple spinlock implemented using test&set instructions
 *
 * @author Michael Schoettner, Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski
 * @date HHU, 2018
 */

#include "kernel/Spinlock.h"

// these methods are implenented in assembler code
extern "C"
{
    void acquireLock(unsigned int* ptr);
    void releaseLock(unsigned int* ptr);
    uint32_t tryAcquireLock(unsigned int* ptr);
    
}

/**
 * Try to get the lock and block until lock is acquired
 */
void Spinlock::lock() {
	acquireLock(ptr);
}

/**
 * Try to acquire the lock once and return if it was successful.
 */
bool Spinlock::tryLock() {
    return tryAcquireLock(ptr) == ZERO_FLAG;
}

/**
 * Unlock the lock.
 */
void Spinlock::unlock() {
    releaseLock(ptr);
}


