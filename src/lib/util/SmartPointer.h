#ifndef HHUOS_SMARTPOINTER_H
#define HHUOS_SMARTPOINTER_H

#include "lib/async/Atomic.h"

namespace Util {

template<typename T>
class SmartPointer {

private:

    T *pointer = nullptr;
    Atomic<uint32_t> *counter;

public:

    SmartPointer();

    explicit SmartPointer(T *pointer);

    SmartPointer(const SmartPointer<T> &copy);

    SmartPointer<T> &operator=(const SmartPointer<T> &other);

    ~SmartPointer();

    T &operator*();

    T *operator->();

    bool operator==(const SmartPointer<T> &other);

    bool operator==(const T *other);

    bool operator==(const T &other);

    bool operator!=(const SmartPointer<T> &other);

    bool operator!=(const T *other);

    bool operator!=(const T &other);

};

template<typename T>
SmartPointer<T>::SmartPointer() {
    this->pointer = nullptr;
    this->counter = new Atomic<uint32_t>();

    counter->fetchAndInc();
}

template<typename T>
SmartPointer<T>::SmartPointer(T *pointer) {
    this->pointer = pointer;
    this->counter = new Atomic<uint32_t>();

    counter->fetchAndInc();
}

template<typename T>
SmartPointer<T>::SmartPointer(const SmartPointer<T> &copy) {
    pointer = copy.pointer;
    counter = copy.counter;

    counter->fetchAndInc();
}

template<typename T>
SmartPointer<T>::~SmartPointer() {
    if (counter->fetchAndDec() == 1) {
        delete pointer;
        delete counter;
    }
}

template<typename T>
SmartPointer<T> &SmartPointer<T>::operator=(const SmartPointer<T> &other) {
    if (this == &other) {
        return *this;
    }

    if (counter->fetchAndDec() == 1) {
        delete pointer;
        delete counter;
    }

    pointer = other.pointer;
    counter = other.counter;

    counter->fetchAndInc();

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
