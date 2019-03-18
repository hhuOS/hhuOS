#ifndef HHUOS_ATOMIC_H
#define HHUOS_ATOMIC_H

#include <cstdint>

template <typename T>
class Atomic {

private:

    T value = 0;

private:

    static T compareAndExchange(volatile void *ptr, T oldValue, T newValue);

    static void exchange(volatile void *ptr, T newValue);

    static void fetchAndAdd(volatile void *ptr, T addend);

public:

    explicit Atomic() = default;

    explicit Atomic(T value);

    Atomic(const Atomic<T> &other);

    Atomic<T>& operator=(const Atomic<T> &other);

    ~Atomic() = default;

    T getAndSet(T newValue);

    T get();

    void set(T newValue);

    void add(T addend);

    void sub(T subtrahend);

    void inc();

    void dec();

};

template class Atomic<int8_t>;
template class Atomic<uint8_t>;
template class Atomic<int16_t>;
template class Atomic<uint16_t>;
template class Atomic<int32_t>;
template class Atomic<uint32_t>;

#endif
