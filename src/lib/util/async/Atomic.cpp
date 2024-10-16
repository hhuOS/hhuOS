#include "Atomic.h"
#include "lib/util/base/Exception.h"

namespace Util::Async {

template<typename T>
Atomic<T>::Atomic(T &value) : value(value) {}

template<typename T>
void Atomic<T>::exchange(volatile void *ptr, T newValue) {
    asm volatile (
    "lock xchg %0, %1"
    :
    : "q"(newValue), "m"(*(volatile T *) ptr)
    : "memory"
    );
}

template<typename T>
T Atomic<T>::fetchAndAdd(volatile void *ptr, T addend) {
    asm volatile (
    "lock xadd %0, %1"
    : "+r" (addend), "+m" (*(volatile T *) ptr)
    :
    : "memory"
    );

    return addend;
}

template<typename T>
T Atomic<T>::compareAndExchange(volatile void *ptr, T oldValue, T newValue) {
    T ret;

    asm volatile (
    "lock cmpxchg %2, %1"
    : "=a"(ret), "+m"(*(volatile T *) ptr)
    : "r"(newValue), "0"(oldValue)
    : "memory"
    );

    return ret;
}

template<typename T>
Atomic<T>::operator T() const {
    return get();
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
    uint32_t oldValue;
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
    return fetchAndAdd(&value, subtrahend * (-1));
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
    uint32_t ret = 0;

    asm volatile (
    "bt %1, %0;"
    : "+m"(value)
    : "r"(index)
    );

    asm volatile (
    "lahf;" : "=a"(ret)
    );

    return (ret & 0x0100) != 0;
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
void Atomic<T>::bitReset(T index) {
    asm volatile (
    "lock btr %1, %0"
    : "+m"(value)
    : "r"(index)
    );
}

template<typename T>
bool Atomic<T>::bitTestAndSet(T index) {
    uint32_t ret = 0;

    asm volatile (
    "lock bts %1, %0;"
    : "+m"(value)
    : "r"(index)
    );

    asm volatile (
    "lahf;" : "=a"(ret)
    );

    return (ret & 0x0100) != 0;
}

template<typename T>
bool Atomic<T>::bitTestAndReset(T index) {
    uint32_t ret = 0;

    asm volatile (
    "lock btr %1, %0;"
    : "+m"(value)
    : "r"(index)
    );

    asm volatile (
    "lahf;" : "=a"(ret)
    );

    return (ret & 0x0100) != 0;
}

template<typename T>
T Atomic<T>::add(T addend) {
    return fetchAndAdd(addend);
}

template<typename T>
T Atomic<T>::sub(T subtrahend) {
    return fetchAndSub(subtrahend);
}

template<typename T>
T Atomic<T>::inc() {
    return fetchAndInc();
}

template<typename T>
T Atomic<T>::dec() {
    return fetchAndDec();
}

template<> bool Atomic<int8_t>::bitTest([[maybe_unused]] int8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> void Atomic<int8_t>::bitSet([[maybe_unused]] int8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> void Atomic<int8_t>::bitReset([[maybe_unused]] int8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<int8_t>::bitTestAndSet([[maybe_unused]] int8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<int8_t>::bitTestAndReset([[maybe_unused]] int8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<uint8_t>::bitTest([[maybe_unused]] uint8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> void Atomic<uint8_t>::bitSet([[maybe_unused]] uint8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> void Atomic<uint8_t>::bitReset([[maybe_unused]] uint8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<uint8_t>::bitTestAndSet([[maybe_unused]] uint8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

template<> bool Atomic<uint8_t>::bitTestAndReset([[maybe_unused]] uint8_t index) {
    Exception::throwException(Exception::INVALID_ARGUMENT, "Bitwise atomic operation are not supported with 8-bit values!");
}

}