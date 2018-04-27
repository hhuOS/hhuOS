#ifndef __Lock_include__
#define __Lock_include__

#include <cstdint>

class Lock {

public:

    Lock() = default;

    Lock(const Lock &other) = delete;

    Lock &operator=(const Lock &other) = delete;

    virtual ~Lock() = default;

    /**
     * Acquires the lock.
     */
    virtual void acquire() = 0;

    /**
     * Releases the lock.
     */
    virtual void release() = 0;

    /**
     * Indicates if the lock is held.
     *
     * @return true, if the lock is held, else false
     */
    virtual bool isLocked() = 0;

    static bool swap(uint32_t *address, uint32_t oldValue, uint32_t newValue);
};


#endif
