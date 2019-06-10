#include "device/cpu/Cpu.h"
#include "Atomic.h"
#include "lib/string/String.h"

template<typename T>
Atomic<T>::Atomic(T value) noexcept {
    set(value);
}

template<typename T>
Atomic<T>::Atomic(const Atomic<T> &other) {
    set(other.value);
}

template<typename T>
Atomic<T> &Atomic<T>::operator=(const Atomic<T> &other) {
    if(this == &other) {
        return *this;
    }

    set(other.value);

    return *this;
}

template<typename T>
void Atomic<T>::exchange(volatile void *ptr, T newValue) {
    asm volatile (
        "lock xchg %0, %1"
        :
        : "q"(newValue), "m"(*(volatile T*)ptr)
        : "memory"
    );
}

template<typename T>
T Atomic<T>::fetchAndAdd(volatile void *ptr, T addend) {
    asm volatile (
        "lock xadd %0, %1"
        : "+r" (addend), "+m" (*(volatile T*)ptr)
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
    : "=a"(ret), "+m"(*(volatile T*)ptr)
    : "r"(newValue), "0"(oldValue)
    : "memory"
    );

    return ret;
}

template<typename T>
T Atomic<T>::getAndSet(T newValue) {
    return compareAndExchange(&value, value, newValue);
}

template<typename T>
T Atomic<T>::get() {
    return value;
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

template<>
bool Atomic<bool>::fetchAndAdd(bool subtrahend) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "Cannot add to a boolean type value!");
}

template<>
bool Atomic<bool>::fetchAndSub(bool subtrahend) {
    Cpu::throwException(Cpu::Exception::UNSUPPORTED_OPERATION, "Cannot subtract from a boolean type value!");
}