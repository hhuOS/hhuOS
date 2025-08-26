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

#ifndef HHUOS_LIB_UTIL_ARRAY_H
#define HHUOS_LIB_UTIL_ARRAY_H

#include <stddef.h>
#include <initializer_list>

#include "base/Address.h"
#include "base/Panic.h"

namespace Util {

/// A heap-allocated array that can hold a fixed number of elements.
/// Bounds checking is performed when accessing elements.
/// This class implements `begin()` and `end()`, allowing iteration over its elements.
///
/// ## Example
/// ```c++
/// auto array = Util::Array<int>({1, 2, 3, 4, 5});
/// array[0] = 10; // Change the first element to 10
///
/// for (const auto &element : array) {
///     Util::System::out << element << " "; // Prints: 10 2 3 4 5
/// }
///
/// Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
/// ```
template <typename T>
class Array {

public:
    /// Create a new array with the given capacity.
    /// The elements of the array are not initialized, so they may contain garbage values.
    ///
    /// ### Example
    /// ```c++
    /// auto array = Util::Array<int>(10); // Create an array with a capacity of 10 elements
    /// for (size_t i = 0; i < array.length(); i++) {
    ///     array[i] = i; // Initialize the array with values 0 to 9
    /// }
    ///
    /// for (const auto &element : array) {
    ///     Util::System::out << element << " "; // Prints: 0 1 2 3 4 5 6 7 8 9
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    /// ```
    explicit Array(size_t capacity = 0);

    /// Create a new array from an initializer list.
    /// The elements of the list are copied to the array and the capacity is set to the size of the list.
    ///
    /// ### Example
    /// ```c++
    /// const auto array = Util::Array<int>({1, 2, 3, 4, 5});
    ///
    /// for (const auto &element : array) {
    ///     Util::System::out << element << " "; // Prints: 1 2 3 4 5
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    /// ```
    Array(std::initializer_list<T> list);

    /// Create a new array by copying the elements of another array.
    Array(const Array &other);

    /// Assign the elements of another array to this array, overwriting the current contents.
    Array& operator=(const Array &other);

    /// Delete the array and free the heap memory.
    ~Array();

    /// Access an element at the given index.
    ///
    /// ### Example
    /// ```c++
    /// const auto array = Util::Array<int>({1, 2, 3, 4, 5});
    /// Util::System::out << array[0] << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush; // Prints: 1
    /// ```
    const T& operator[](size_t index) const;

    /// Access a reference to an element at the given index, allowing modification of the element.
    ///
    /// ### Example
    /// ```c++
    /// const auto array = Util::Array<int>({1, 2, 3, 4, 5});
    /// array[0] = 10; // Change the first element to 10
    /// ```
    T& operator[](size_t index);

    /// Get the number of elements in the array.
    [[nodiscard]] size_t length() const;

    /// Check if the array contains the given element.
    ///
    /// ### Example
    /// ```c++
    /// const auto array = Util::Array<int>({1, 2, 3, 4, 5});
    /// bool containsTwo = array.contains(2); // true
    /// bool containsSix = array.contains(6); // false
    /// ```
    [[nodiscard]] bool contains(const T &element);

    /// Get a pointer to the beginning of the array.
    /// This allows iteration over the elements of the array using a range-based for loop.
    [[nodiscard]] T* begin() const;

    /// Get a pointer to the end of the array.
    /// This allows iteration over the elements of the array using a range-based for loop.
    [[nodiscard]] T* end() const;

private:

    T* array;
    size_t capacity;
};

template <class T>
Array<T>::Array(const size_t capacity) : capacity(capacity) {
    array = new T[capacity];
}

template <class T>
Array<T>::Array(std::initializer_list<T> list) : capacity(list.size()) {
    array = new T[list.size()];

    const T *source = list.begin();
    for (size_t i = 0; i < capacity; i++) {
        array[i] = source[i];
    }
}

template <class T>
Array<T>::Array(const Array &other) : capacity(other.capacity) {
    array = new T[capacity];

    for (uint32_t i = 0; i < capacity; i++) {
        array[i] = other.array[i];
    }
}

template <class T>
Array<T>& Array<T>::operator=(const Array &other) {
    if (&other == this) {
        return *this;
    }

    delete[] array;
    capacity = other.capacity;
    array = new T[capacity];

    for (uint32_t i = 0; i < capacity; i++) {
        array[i] = other.array[i];
    }

    return *this;
}

template<typename T>
Array<T>::~Array() {
    delete[] array;
}

template <class T>
T &Array<T>::operator[](size_t index) {
    if (index >= capacity) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "Array: Index out of bounds!");
    }

    return array[index];
}

template <class T>
const T &Array<T>::operator[](size_t index) const {
    if (index >= capacity) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "Array: Index out of bounds!");
    }

    return array[index];
}

template <class T>
size_t Array<T>::length() const {
    return capacity;
}

template<typename T>
bool Array<T>::contains(const T &element) {
    for (const auto &value : *this) {
        if (value == element) {
            return true;
        }
    }

    return false;
}

template<typename T>
T* Array<T>::begin() const {
    return array;
}

template<typename T>
T* Array<T>::end() const {
    return array + capacity;
}

}

#endif
