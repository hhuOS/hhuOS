#ifndef HHUOS_LIB_UTIL_ASYNC_LOCK_H
#define HHUOS_LIB_UTIL_ASYNC_LOCK_H

namespace Util::Async {

/// Base class for a lock, which can be used to synchronize access to shared resources.
class Lock {

public:
    /// The Lock base class has no state, so the default constructor is sufficient.
    Lock() = default;

    /// The Lock base class has no state, so the default destructor is sufficient.
    virtual ~Lock() = default;

    /// Locks should not be copied, since copies would not operate on the same value.
    Lock(const Lock &other) = delete;

    /// Locks should not be copied, since copies would not operate on the same value.
    Lock& operator=(const Lock &other) = delete;

    /// Acquire the lock.
    /// This function blocks until the lock is available.
    virtual void acquire() = 0;

    /// Try to acquire the lock once.
    /// If the lock is not available, the function does not block and returns false.
    virtual bool tryAcquire() = 0;

    /// Release the lock.
    /// If the lock is not held, this function does nothing.
    virtual void release() = 0;

    /// Check if the lock is currently held.
    virtual bool isLocked() const = 0;

    /// Release the lock and return a given value.
    /// This function is useful for returning a value from a function that also releases the lock.
    ///
    /// ### Example
    /// ```c++
    /// int value = 0 // Global variable
    /// Util::Async::Spinlock lock; // Spinlock implementation
    ///
    /// // Function that runs in a thread and manipulates the value.
    /// int threadFunction() {
    ///     lock.acquire();
    ///     value++;
    ///     lock.release();
    ///
    ///     return value;
    /// }
    ///
    /// // We can shorten the code by using the releaseAndReturn function.
    /// int threadFunction() {
    ///     lock.acquire();
    ///     value++;
    ///     return lock.releaseAndReturn(value);
    /// }
    /// ```
    template<typename T>
    T releaseAndReturn(T returnValue) {
        release();
        return returnValue;
    }
};

}

#endif
