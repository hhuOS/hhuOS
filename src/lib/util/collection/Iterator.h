/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_LIB_UTIL_ITERATOR_H
#define HHUOS_LIB_UTIL_ITERATOR_H

#include <stddef.h>

namespace Util {

/// An element of an iterator, containing a pointer to the data and the index in the iterable.
/// The iterator design assumes, that the next element can be accessed from the current element,
/// either by incrementing the index or by using a pointer to the next element.
template <typename T>
struct IteratorElement {
    /// Pointer to the data of the current element in the iterable collection.
    T *data;
    /// The index of the current element in the iterable collection.
    size_t index;
};

template <typename T>
class Iterator;

/// Base class for iterable collections.
template <typename T>
class Iterable {

public:
    /// The Iterable base class has no state, so the default constructor is sufficient.
    Iterable() = default;

    /// The Iterable base class has no state, so the default destructor is sufficient.
    virtual ~Iterable() = default;

    /// Get an iterator for the collection, starting at the first element.
    virtual Iterator<T> begin() const = 0;

    /// Get an iterator for the collection, pointing to the end (one past the last element).
    virtual Iterator<T> end() const = 0;

    /// Get the next element in the iteration based on the current element.
    virtual IteratorElement<T> next(const IteratorElement<T> &element) const = 0;
};


/// An iterator for iterable collections.
/// The iterator works by wrapping elements of the collection in an `IteratorElement` struct.
/// Each `IteratorElement` contains a pointer to the data and the index of the element in the collection.
/// The iterator design assumes, that the next element can be accessed from the current element,
/// either by incrementing the index or by using a pointer to the next element.
template <typename T>
class Iterator {

public:
    /// Create a new iterator for the given iterable collection and the initial element.
    Iterator(const Iterable<T> &iterable, const IteratorElement<T> &element);

    /// Compare this iterator with another iterator for inequality.
    /// This implementation checks if the indices of the elements are different.
    bool operator!=(const Iterator &other);

    /// Advance the iterator to the next element in the iterable collection.
    Iterator& operator++();

    /// Get a reference to the current element in the iteration.
    T& operator*();

private:

    const Iterable<T> &iterable;
    IteratorElement<T> element;
};

template<typename T>
Iterator<T>::Iterator(const Iterable<T> &iterable, const IteratorElement<T> &element) :
    iterable(iterable), element(element) {}

template <class T>
T& Iterator<T>::operator*() {
    return *element.data;
}

template <class T>
bool Iterator<T>::operator!=(const Iterator &other) {
    return element.index != other.element.index;
}

template <class T>
Iterator<T>& Iterator<T>::operator++() {
    element = iterable.next(element);
    return *this;
}

}


#endif
