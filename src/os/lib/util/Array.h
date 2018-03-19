#ifndef __Array_include__
#define __Array_include__

#include <cstdint>
#include <kernel/Cpu.h>

extern "C" {
    #include "lib/libc/string.h"
};

namespace Util {

    template <typename T>
    class Array {

    public:

        explicit Array(uint32_t capacity);

        virtual ~Array() = default;

        Array(const Array<T> &other);

        Array<T> &operator=(const Array<T> &other);

        T &operator[](uint32_t index);

        const T &operator[](uint32_t index) const;

        void clear();

        uint32_t length() const;

        T *begin() const;

        T *end() const;

    private:

        T* array;

        uint32_t capacity;

    };

    template <class T>
    Array<T>::Array(uint32_t capacity) : capacity(capacity) {

        this->array = new T[capacity];
    }

    template <class T>
    Array<T>::Array(const Array<T> &other) {

        capacity = other.capacity;

        array = new T[capacity];

        for (uint32_t i = 0; i < capacity; i++) {
            array[i] = other.array[i];
        }
    }

    template <class T>
    Array<T> &Array<T>::operator=(const Array<T> &other) {

        delete[] array;

        capacity = other.capacity;

        array = new T[capacity];

        for (uint32_t i = 0; i < capacity; i++) {
            array[i] = other.array[i];
        }

        return *this;
    }

    template <class T>
    T &Array<T>::operator[](uint32_t index) {

        if (index >= capacity) {
            Cpu::throwException(Cpu::Exception::OUTOFBOUNDS);
        }

        return array[index];
    }

    template <class T>
    const T &Array<T>::operator[](uint32_t index) const {

        if (index >= capacity) {
            Cpu::throwException(Cpu::Exception::OUTOFBOUNDS);
        }

        return array[index];
    }

    template <class T>
    uint32_t Array<T>::length() const {

        return capacity;
    }

    template <class T>
    T *Array<T>::begin() const {

        return array;
    }

    template <class T>
    T *Array<T>::end() const {

        return &array[capacity];
    }

    template <class T>
    void Array<T>::clear() {

        delete[] array;

        array = new T[capacity];
    }
}

#endif
