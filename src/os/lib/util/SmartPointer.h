#ifndef HHUOS_SMARTPOINTER_H
#define HHUOS_SMARTPOINTER_H

#include <cstdint>
#include <lib/lock/Spinlock.h>

namespace Util {

template<typename T>
class SmartPointer {

private:

    struct ReferenceCounter {

    private:

        uint32_t counter = 0;

    public:

        uint32_t addReference() {
            return ++counter;
        }

        uint32_t release() {
            return --counter;
        }
    };

private:

    T *pointer = nullptr;
    ReferenceCounter *counter = nullptr;
    Spinlock *lock = nullptr;

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
    this->counter = new ReferenceCounter();
    this->lock = new Spinlock();

    lock->acquire();

    counter->addReference();

    lock->release();
}

template<typename T>
SmartPointer<T>::SmartPointer(T *pointer) {
    this->pointer = pointer;
    this->counter = new ReferenceCounter();
    this->lock = new Spinlock();

    lock->acquire();

    counter->addReference();

    lock->release();
}

template<typename T>
SmartPointer<T>::SmartPointer(const SmartPointer<T> &copy) {
    pointer = copy.pointer;
    counter = copy.counter;
    lock = copy.lock;

    lock->acquire();

    counter->addReference();

    lock->release();
}

template<typename T>
SmartPointer<T>::~SmartPointer() {
    lock->acquire();

    if (counter->release() == 0) {
        delete pointer;
        delete counter;
        delete lock;
    } else {
        lock->release();
    }
}

template<typename T>
SmartPointer<T> &SmartPointer<T>::operator=(const SmartPointer<T> &other) {
    if (this == &other) {
        return *this;
    }

    lock->acquire();

    if (counter->release() == 0) {
        delete pointer;
        delete counter;
        delete lock;
    } else {
        lock->release();
    }

    pointer = other.pointer;
    counter = other.counter;
    lock = other.lock;

    lock->acquire();

    counter->addReference();

    lock->release();

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
