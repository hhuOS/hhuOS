#ifndef HHUOS_SMARTPOINTER_H
#define HHUOS_SMARTPOINTER_H

#include "lib/async/Spinlock.h"

namespace Util {

template<typename T>
class SmartPointer {

private:

    T *pointer = nullptr;
    uint32_t counter;

    Async::Spinlock lock;

public:

    SmartPointer();

    explicit SmartPointer(T *pointer);

    SmartPointer(const SmartPointer<T> &copy);

    SmartPointer<T> &operator=(const SmartPointer<T> &other);

    ~SmartPointer();

    T* get();

    T& operator*();

    T* operator->();

    bool operator==(const SmartPointer<T> &other);

    bool operator==(const T *other);

    bool operator==(const T &other);

    bool operator!=(const SmartPointer<T> &other);

    bool operator!=(const T *other);

    bool operator!=(const T &other);

};

template<typename T>
SmartPointer<T>::SmartPointer() {
    lock.acquire();

    pointer = nullptr;
    counter = 1;

    lock.release();
}

template<typename T>
SmartPointer<T>::SmartPointer(T *pointer) : pointer(pointer), counter(0) {
    lock.acquire();

    this->pointer = pointer;
    counter = 1;

    lock.release();
}

template<typename T>
SmartPointer<T>::SmartPointer(const SmartPointer<T> &copy) {
    lock.acquire();

    pointer = copy.pointer;
    counter = copy.counter;
    counter++;

    lock.release();
}

template<typename T>
SmartPointer<T>::~SmartPointer() {
    lock.acquire();

    if (counter-- == 0) {
        lock.release();
        delete pointer;

        return;
    }

    lock.release();
}

template<typename T>
T * SmartPointer<T>::get() {
    return pointer;
}

template<typename T>
SmartPointer<T> &SmartPointer<T>::operator=(const SmartPointer<T> &other) {
    if (this == &other) {
        return *this;
    }

    lock.acquire();

    if (counter-- == 0) {
        delete pointer;
    }

    pointer = other.pointer;
    counter = other.counter;

    counter++;

    lock.release();

    return *this;
}

template<typename T>
T &SmartPointer<T>::operator*() {
    return *pointer;
}

template<typename T>
T *SmartPointer<T>::operator->() {
    return pointer;
}

template<typename T>
bool SmartPointer<T>::operator==(const SmartPointer<T> &other) {
    return pointer == other.pointer;
}

template<typename T>
bool SmartPointer<T>::operator==(const T *other) {
    return pointer == other;
}

template<typename T>
bool SmartPointer<T>::operator==(const T &other) {
    return pointer == &other;
}

template<typename T>
bool SmartPointer<T>::operator!=(const SmartPointer<T> &other) {
    return pointer != other.pointer;
}

template<typename T>
bool SmartPointer<T>::operator!=(const T *other) {
    return pointer != other;
}

template<typename T>
bool SmartPointer<T>::operator!=(const T &other) {
    return pointer != &other;
}

}

#endif
