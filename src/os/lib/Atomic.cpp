#include <devices/cpu/Cpu.h>
#include "Atomic.h"
#include "String.h"

template<typename T>
Atomic<T>::Atomic(T value) {
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
        "xchg %0, %1"
        :
        : "q"(newValue), "m"(*(volatile T*)ptr)
        : "memory"
    );
}

template<typename T>
void Atomic<T>::fetchAndAdd(volatile void *ptr, T addend) {
    asm volatile (
        "lock; xadd %0, %1"
        : "+r" (addend), "+m" (*(volatile T*)ptr)
        :
        : "memory"
    );
}

template<typename T>
T Atomic<T>::compareAndExchange(volatile void *ptr, T oldValue, T newValue) {
    T ret;

    asm volatile (
    "lock cmpxchg %0, %1"
    : "=a"(ret), "=m"(*(volatile T*)ptr)
    : "0"(newValue), "q"(oldValue), "m"(*(volatile T*)ptr)
    : "memory"
    );

    return ret;
}

template<typename T>
T Atomic<T>::get() {
    return compareAndExchange(&value, value, value);
}

template<typename T>
void Atomic<T>::set(T newValue) {
    exchange(&value, newValue);
}

template<typename T>
void Atomic<T>::add(T addend) {
    fetchAndAdd(&value, addend);
}

template<typename T>
void Atomic<T>::sub(T subtrahend) {
    fetchAndAdd(&value, subtrahend * (-1));
}

template<typename T>
void Atomic<T>::inc() {
    fetchAndAdd(&value, 1);
}

template<typename T>
void Atomic<T>::dec() {
    fetchAndAdd(&value, -1);
}

template<typename T>
bool Atomic<T>::operator==(Atomic<T> &other) {
    return get() == other.get();
}

template<typename T>
bool Atomic<T>::operator!=(Atomic<T> &other) {
    return get() != other.get();
}

template<typename T>
bool Atomic<T>::operator>(Atomic<T> &other) {
    return get() > other.get();
}

template<typename T>
bool Atomic<T>::operator<(Atomic<T> &other) {
    return get() < other.get();
}

template<typename T>
bool Atomic<T>::operator>=(Atomic<T> &other) {
    return get() >= other.get();
}

template<typename T>
bool Atomic<T>::operator<=(Atomic<T> &other) {
    return get() <= other.get();
}
