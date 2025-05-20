#include "Atomic.h"

#include "base/Panic.h"

namespace Util::Async {

template<typename T>
Atomic<T>::Atomic(T &value) : value(value) {}

template<typename T>
void Atomic<T>::exchange(volatile void *ptr, T newValue) {
    asm volatile (
    "lock xchg %0, %1"
    : "+r"(newValue), "+m"(*static_cast<volatile T*>(ptr))
    );
}

template<typename T>
T Atomic<T>::fetchAndAdd(volatile void *ptr, T addend) {
    asm volatile (
    "lock xadd %0, %1"
    : "+r" (addend), "+m"(*static_cast<volatile T*>(ptr))
    );

    return addend;
}

template<typename T>
T Atomic<T>::compareAndExchange(volatile void *ptr, T oldValue, T newValue) {
    T ret;

    asm volatile (
    "lock cmpxchg %2, %1"
    : "=a"(ret), "+m"(*static_cast<volatile T*>(ptr))
    : "r"(newValue), "0"(oldValue)
    );

    return ret;
}

template<typename T>
T Atomic<T>::get() const {
    return value;
}

template<typename T>
bool Atomic<T>::compareAndSet(T expectedValue, T newValue) {
    return compareAndExchange(&value, expectedValue, newValue) == expectedValue;
}

template<typename T>
T Atomic<T>::getAndSet(T newValue) {
    T oldValue;
    do {
        oldValue = get();
    } while (!compareAndSet(oldValue, newValue));

    return oldValue;
}

template<typename T>
void Atomic<T>::set(T newValue) {
    exchange(&value, newValue);
}

template<typename T>
T Atomic<T>::fetchAndAdd(T addend) {
    return fetchAndAdd(&value, addend);
}

template<typename T>
T Atomic<T>::fetchAndSub(T subtrahend) {
    return fetchAndAdd(&value, -subtrahend);
}

template<typename T>
T Atomic<T>::fetchAndInc() {
    return fetchAndAdd(&value, 1);
}

template<typename T>
T Atomic<T>::fetchAndDec() {
    return fetchAndAdd(&value, -1);
}

template<typename T>
bool Atomic<T>::bitTest(T index) {
    uint8_t flags = 0;

    asm volatile (
    "bt %2, %1;"
    "lahf;"
    "shr $8, %%eax;"
    : "=a"(flags)
    : "m"(value), "r"(index)
    );

    return (flags & 0x0100) != 0;
}

template<typename T>
void Atomic<T>::bitSet(T index) {
    asm volatile (
    "lock bts %1, %0"
    : "+m"(value)
    : "r"(index)
    );
}

template<typename T>
void Atomic<T>::bitUnset(T index) {
    asm volatile (
    "lock btr %1, %0"
    : "+m"(value)
    : "r"(index)
    );
}

template<typename T>
bool Atomic<T>::bitTestAndSet(T index) {
    uint8_t flags = 0;

    asm volatile (
    "lock bts %2, %0;"
    "lahf;"
    "shr $8, %%eax;"
    : "+m"(value), "=a"(flags)
    : "r"(index)
    );

    // Old bit value is stored in the carry flag
    return (flags & 0x01) != 0;
}

template<typename T>
bool Atomic<T>::bitTestAndUnset(T index) {
    uint8_t flags = 0;

    asm volatile (
    "lock btr %2, %0;"
    "lahf;"
    "shr $8, %%eax;"
    : "+m"(value), "=a"(flags)
    : "r"(index)
    );

    // Old bit value is stored in the carry flag
    return (flags & 0x01) != 0;
}

template<typename T>
void Atomic<T>::add(T addend) {
    fetchAndAdd(&value, addend);
}

template<typename T>
void Atomic<T>::sub(T subtrahend) {
    fetchAndAdd(&value, -subtrahend);
}

template<typename T>
void Atomic<T>::inc() {
    fetchAndAdd(&value, 1);
}

template<typename T>
void Atomic<T>::dec() {
    fetchAndAdd(&value, -1);
}

template<> bool Atomic<int8_t>::bitTest([[maybe_unused]] int8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> void Atomic<int8_t>::bitSet([[maybe_unused]] int8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> void Atomic<int8_t>::bitUnset([[maybe_unused]] int8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<int8_t>::bitTestAndSet([[maybe_unused]] int8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<int8_t>::bitTestAndUnset([[maybe_unused]] int8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<uint8_t>::bitTest([[maybe_unused]] uint8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> void Atomic<uint8_t>::bitSet([[maybe_unused]] uint8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> void Atomic<uint8_t>::bitUnset([[maybe_unused]] uint8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<uint8_t>::bitTestAndSet([[maybe_unused]] uint8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<uint8_t>::bitTestAndUnset([[maybe_unused]] uint8_t index) {
    Panic::fire(Panic::INVALID_ARGUMENT,
        "Bitwise atomic operation are not supported with 8-bit values!");
}

}
