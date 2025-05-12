#ifndef HHUOS_LIB_UTIL_ASYNC_ATOMIC_H
#define HHUOS_LIB_UTIL_ASYNC_ATOMIC_H

#include <stdint.h>

namespace Util::Async {

/// Atomic operations for 8, 16, and 32-bit integers.
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
///         printf("Incremented value from [%u] to [%u]: %u\n", oldValue, oldValue + 1);
///     }
/// }
/// ```
template<typename T>
class Atomic {

public:
    /// Create a new Atomic object with a reference to the value, which shall be accessed atomically.
    explicit Atomic(T &value);

    /// Get the value.
    [[nodiscard]] T get() const;

    /// Compare the value with the expected value and set it to the new value if they are equal.
    bool compareAndSet(T expectedValue, T newValue);

    /// Set the value to the new value and return the old value.
    [[nodiscard]] T getAndSet(T newValue);

    /// Set the value to the new value.
    void set(T newValue);

    /// Add the addend to the value and return the old value.
    [[nodiscard]] T fetchAndAdd(T addend);

    /// Subtract the subtrahend from the value and return the old value.
    [[nodiscard]] T fetchAndSub(T subtrahend);

    /// Increment the value and return the old value.
    [[nodiscard]] T fetchAndInc();

    /// Decrement the value and return the old value.
    [[nodiscard]] T fetchAndDec();

    /// Add the addend to the value.
    void add(T addend);

    /// Subtract the subtrahend from the value.
    void sub(T subtrahend);

    /// Increment the value.
    void inc();

    /// Decrement the value.
    void dec();

    /// Check if the bit at the given index is set. Returns true if the bit is set, false otherwise.
    /// The index must be within the valid range of the value type, otherwise behavior is undefined.
    /// Only 16 and 32-bit values are supported.
    [[nodiscard]] bool bitTest(T index);

    /// Set the bit at the given index to 1.
    /// Only 16 and 32-bit values are supported.
    void bitSet(T index);

    /// Set the bit at the given index to 0.
    /// Only 16 and 32-bit values are supported.
    void bitUnset(T index);

    /// Set the bit at the given index to 1 and return the old value of the bit (true if it was set, false otherwise).
    /// Only 16 and 32-bit values are supported.
    [[nodiscard]] bool bitTestAndSet(T index);

    /// Set the bit at the given index to 0 and return the old value of the bit (true if it was set, false otherwise).
    /// Only 16 and 32-bit values are supported.
    [[nodiscard]] bool bitTestAndUnset(T index);

private:

    static void exchange(volatile void *ptr, T newValue);
    static T compareAndExchange(volatile void *ptr, T oldValue, T newValue);
    static T fetchAndAdd(volatile void *ptr, T addend);

    T &value;

};

template class Atomic<int8_t>;
template class Atomic<uint8_t>;
template class Atomic<int16_t>;
template class Atomic<uint16_t>;
template class Atomic<int32_t>;
template class Atomic<uint32_t>;

}

#endif
