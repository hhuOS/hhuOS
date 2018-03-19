#ifndef __Iterator_include__
#define __Iterator_include__

#include "Array.h"

namespace Util {

    template <typename T>
    class Iterator {

    public:

        explicit Iterator(Array<T> array, uint32_t index);

        Iterator(const Iterator<T> &other);

        Iterator<T> &operator=(const Iterator<T> &other);

        ~Iterator() = default;

        bool operator!=(const Iterator<T> &other);

        const Iterator<T> &operator++();

        T &operator*();

        bool hasNext() const;

        T &next() const;

    private:

        Array<T> array;

        mutable uint32_t index = 0;

    };

    template <class T>
    Iterator<T>::Iterator(Array<T> array, uint32_t index) : array(array), index(index) {

    }

    template <class T>
    Iterator<T>::Iterator(const Iterator<T> &other) : array(other.array), index(other.index) {

    }

    template <class T>
    Iterator<T> &Iterator<T>::operator=(const Iterator<T> &other) {

        array = other.array;

        index = other.index;

        return *this;
    }

    template <class T>
    T &Iterator<T>::operator*() {

        return array[index];
    }

    template <class T>
    bool Iterator<T>::operator!=(const Iterator<T> &other) {

        return index != other.index;
    }

    template <class T>
    const Iterator<T> &Iterator<T>::operator++() {

        index++;

        return *this;
    }

    template <class T>
    bool Iterator<T>::hasNext() const {

        return index < array.length();
    }

    template <class T>
    T &Iterator<T>::next() const {

        index++;

        return array[index - 1];
    }


}


#endif
