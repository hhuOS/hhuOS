/**
 * Spinlock - a simple spinlock implemented using test&set instructions
 *
 * @author Michael Schoettner, Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski
 * @date HHU, 2018
 */

#ifndef __Spinlock_include__
#define __Spinlock_include__


#include <cstdint>

class Spinlock {
    
private:
	// the value which is used for locking
    unsigned int lock_var;
    // pointer to the lock-value
    unsigned int *ptr;

public:

    Spinlock(const Spinlock &other) = delete;
    /**
     * Constructor - initializes Spinlock.
     */
    Spinlock() noexcept {
        lock_var = 0;
        ptr  = &lock_var;
    }

    ~Spinlock() = default;

    /**
     * Try to get the lock and block until lock is acquired
     */
    void lock();
    
    /**
	 * Unlock the lock.
	 */
    void unlock();

    /**
     * Try to acquire the lock once and return if it was successful.
     *
     * @return true = successful, false = not successful
     */
    bool tryLock();

    // Position of the zero flag in EFLAG
    const static uint32_t ZERO_FLAG = 0x40;
};

#endif

