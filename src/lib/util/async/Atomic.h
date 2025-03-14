#ifndef HHUOS_ATOMIC_H
#define HHUOS_ATOMIC_H

#include <stdint.h>

namespace Util::Async {

template<typename T>
class Atomic {

public:

    explicit Atomic(T &value);

    Atomic(const Atomic<T> &other) = delete;

    Atomic<T> &operator=(const Atomic<T> &other) = delete;

    ~Atomic() = default;

    explicit operator T() const;

    [[nodiscard]] T get() const;

    bool compareAndSet(T expectedValue, T newValue);

    [[nodiscard]] T getAndSet(T newValue);

    void set(T newValue);

    [[nodiscard]] T fetchAndAdd(T addend);

    [[nodiscard]] T fetchAndSub(T subtrahend);

    [[nodiscard]] T fetchAndInc();

    [[nodiscard]] T fetchAndDec();

    T add(T addend);

    T sub(T subtrahend);

    T inc();

    T dec();

    [[nodiscard]] bool bitTest(T index);

    void bitSet(T index);

    void bitReset(T index);

    [[nodiscard]] bool bitTestAndSet(T index);

    [[nodiscard]] bool bitTestAndReset(T index);

private:

    T &value;

    static T compareAndExchange(volatile void *ptr, T oldValue, T newValue);

    static void exchange(volatile void *ptr, T newValue);

    static T fetchAndAdd(volatile void *ptr, T addend);

};

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
