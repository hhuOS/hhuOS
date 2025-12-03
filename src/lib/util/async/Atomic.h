#ifndef HHUOS_LIB_UTIL_ASYNC_ATOMIC_H
#define HHUOS_LIB_UTIL_ASYNC_ATOMIC_H

#include <stdint.h>

#include "util/base/Panic.h"

namespace Util {
namespace Async {

/// Atomic operations for 16 and 32-bit integers.
/// This class does not own the value, it rather provides atomic operations on a reference to an existing value.
///
/// ## Example
///
/// ```c++
/// int value = 0; // Global variable
///
/// // Function that runs in a thread.
/// // The atomic wrapper ensures that the value is accessed atomically.
/// // The final value should be 1000 times the amount of threads executing this function.
/// // If we would not use the atomic wrapper, the final value would be undefined due to the `Lost Update` problem.
/// void threadFunction() {
///     auto atomicWrapper = Util::Async::Atomic<int>(value);
///
///     for (int i = 0; i < 1000; i++) {
///         const auto oldValue = atomicWrapper.inc();
///         Util::System::out << "Incremented value from " << oldValue << " to " << (oldValue + 1)
///             << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
///     }
/// }
/// ```
template<typename T>
class Atomic {

public:
    /// Create a new Atomic object with a reference to the value, which shall be accessed atomically.
    explicit Atomic(T &value) : value(value) {}

    /// Get the value.
    T get() const {
        return value;
    }

    /// Compare the value with the expected value and set it to the new value if they are equal.
    bool compareAndSet(T expectedValue, T newValue) {
        return compareAndExchange(&value, expectedValue, newValue) == expectedValue;
    }

    /// Set the value to the new value and return the old value.
    T getAndSet(T newValue) {
        T oldValue;
        do {
            oldValue = get();
        } while (!compareAndSet(oldValue, newValue));

        return oldValue;
    }

    /// Set the value to the new value.
    void set(T newValue) {
        exchange(&value, newValue);
    }

    /// Add the addend to the value and return the old value.
    T fetchAndAdd(T addend) {
        return fetchAndAdd(&value, addend);
    }

    /// Subtract the subtrahend from the value and return the old value.
    T fetchAndSub(T subtrahend) {
        return fetchAndAdd(&value, -subtrahend);
    }

    /// Increment the value and return the old value.
    T fetchAndInc() {
        return fetchAndAdd(&value, 1);
    }

    /// Decrement the value and return the old value.
    T fetchAndDec() {
        return fetchAndAdd(&value, -1);
    }

    /// Add the addend to the value.
    void add(T addend) {
        fetchAndAdd(&value, addend);
    }

    /// Subtract the subtrahend from the value.
    void sub(T subtrahend) {
        fetchAndAdd(&value, -subtrahend);
    }

    /// Increment the value.
    void inc() {
        fetchAndAdd(&value, 1);
    }

    /// Decrement the value.
    void dec() {
        fetchAndAdd(&value, -1);
    }

    /// Check if the bit at the given index is set. Returns true if the bit is set, false otherwise.
    /// The index must be within the valid range of the value type, otherwise behavior is undefined.
    bool bitTest(T index) {
        uint8_t flags = 0;

        asm volatile (
        "bt %2, %1;"
        "lahf;"
        "shr $8, %%eax;"
        : "=a"(flags)
        : "m"(value), "r"(index)
        );

        return flags & 0x01;
    }

    /// Set the bit at the given index to 1.
    void bitSet(T index) {
        asm volatile (
        "lock bts %1, %0"
        : "+m"(value)
        : "r"(index)
        );
    }

    /// Set the bit at the given index to 0.
    void bitUnset(T index) {
        asm volatile (
        "lock btr %1, %0"
        : "+m"(value)
        : "r"(index)
        );
    }

    /// Set the bit at the given index to 1 and return the old value of the bit (true if it was set, false otherwise).
    bool bitTestAndSet(T index) {
        uint8_t flags = 0;

        asm volatile (
        "lock bts %2, %0;"
        "lahf;"
        "shr $8, %%eax;"
        : "+m"(value), "=a"(flags)
        : "r"(index)
        );

        // Old bit value is stored in the carry flag
        return flags & 0x01;
    }

    /// Set the bit at the given index to 0 and return the old value of the bit (true if it was set, false otherwise).
    bool bitTestAndUnset(T index) {
        uint8_t flags = 0;

        asm volatile (
        "lock btr %2, %0;"
        "lahf;"
        "shr $8, %%eax;"
        : "+m"(value), "=a"(flags)
        : "r"(index)
        );

        // Old bit value is stored in the carry flag
        return flags & 0x01;
    }

private:

    static void exchange(volatile void *ptr, T newValue) {
        asm volatile (
        "lock xchg %0, %1"
        : "+r"(newValue), "+m"(*static_cast<volatile T*>(ptr))
        );
    }

    static T compareAndExchange(volatile void *ptr, T oldValue, T newValue) {
        T ret;

        asm volatile (
        "lock cmpxchg %2, %1"
        : "=a"(ret), "+m"(*static_cast<volatile T*>(ptr))
        : "r"(newValue), "0"(oldValue)
        );

        return ret;
    }

    static T fetchAndAdd(volatile void *ptr, T addend) {
        asm volatile (
        "lock xadd %0, %1"
        : "+r" (addend), "+m"(*static_cast<volatile T*>(ptr))
        );

        return addend;
    }

    T &value;
};

template class Atomic<int16_t>;
template class Atomic<uint16_t>;
template class Atomic<int32_t>;
template class Atomic<uint32_t>;

}
}

#endif
