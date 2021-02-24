#ifndef HHUOS_ATOMIC_H
#define HHUOS_ATOMIC_H

#include <cstdint>

namespace Async {

template<typename T>
class Atomic {

private:

    T value = 0;

private:

    static T compareAndExchange(volatile void *ptr, T oldValue, T newValue);

    static void exchange(volatile void *ptr, T newValue);

    static T fetchAndAdd(volatile void *ptr, T addend);

public:

    explicit Atomic() noexcept = default;

    explicit Atomic(T value) noexcept;

    Atomic(const Atomic<T> &other);

    Atomic<T> &operator=(const Atomic<T> &other);

    ~Atomic() = default;

    explicit operator T() const;

    T get() const;

    T getAndSet(T newValue);

    void set(T newValue);

    T fetchAndAdd(T addend);

    T fetchAndSub(T subtrahend);

    T fetchAndInc();

    T fetchAndDec();

};

template
class Atomic<bool>;

template
class Atomic<int8_t>;

template
class Atomic<uint8_t>;

template
class Atomic<int16_t>;

template
class Atomic<uint16_t>;

template
class Atomic<int32_t>;

template
class Atomic<uint32_t>;

}

#endif
