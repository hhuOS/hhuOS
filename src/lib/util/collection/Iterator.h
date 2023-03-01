/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __Iterator_include__
#define __Iterator_include__

#include "Array.h"

namespace Util {

/**
 * An iterator over an array.
 *
 * @author Filip Krakowski
 */
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

    [[nodiscard]] bool hasNext() const;

    [[nodiscard]] T &next() const;

private:

    Array<T> array;
    mutable uint32_t index = 0;
};

template <class T>
Iterator<T>::Iterator(Array<T> array, uint32_t index) : array(array), index(index) {}

template <class T>
Iterator<T>::Iterator(const Iterator<T> &other) : array(other.array), index(other.index) {}

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
