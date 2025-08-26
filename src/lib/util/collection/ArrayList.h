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

#ifndef HHUOS_LIB_UTIL_ARRAY_LIST_H
#define HHUOS_LIB_UTIL_ARRAY_LIST_H

#include <stddef.h>

#include "collection/List.h"
#include "collection/Iterator.h"

namespace Util {

/// A list implementation based on a dynamically growing array.
///
/// ## Example
/// ```c++
/// auto list = Util::ArrayList<int>();
/// list.add(1); // Append 1 to the list -> 1
/// list.add(2); // Append 2 to the list -> 1, 2
/// list.add(3); // Append 3 to the list -> 1, 2, 3
/// list.addIndex(1, 4); // Insert 4 at index 1 -> 1, 4, 2, 3
/// list.remove(2); // Remove the first occurrence of 2 -> 1, 4, 3
///
/// for (const auto &element : list) {
///     Util::System::out << element << " "; // Prints: 1 4 3
/// }
///
/// Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
/// ```
template <typename T>
class ArrayList final : public List<T> {

public:
    /// Create a new empty ArrayList instance with a default capacity of 0.
    ArrayList() = default;

    /// Create a new empty ArrayList instance with the given initial capacity.
    explicit ArrayList(size_t capacity);

    /// Create a new ArrayList instance by copying the elements from an Array.
    ///
    /// ### Example
    /// ```c++
    /// const auto array = Util::Array<int>({1, 2, 3, 4, 5});
    /// const auto list = Util::ArrayList<int>(array);
    ///
    /// for (const auto &element : list) {
    ///     Util::System::out << element << " "; // Prints: 1 2 3 4 5
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    /// ```
    explicit ArrayList(const Array<T> &elements);

    /// Create a new ArrayList from an initializer list.
    ///
    /// ### Example
    /// ```c++
    /// const auto list = Util::ArrayList<int>({1, 2, 3, 4, 5});
    ///
    /// for (const auto &element : list) {
    ///     Util::System::out << element << " "; // Prints: 1 2 3 4 5
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
    /// ```
    ArrayList(const std::initializer_list<T> &list);

    /// ArrayList is not copyable, so the copy constructor is deleted.
    ArrayList(const ArrayList &other) = delete;

    /// ArrayList is not assignable, so the assignment operator is deleted.
    ArrayList &operator=(const ArrayList &other) = delete;

    /// Delete the list and free the heap memory.
    ~ArrayList() override;

    /// Add an element to the end of the list by copying it.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>();
    /// list.add(1); // Append 1 to the list -> 1
    /// list.add(2); // Append 2 to the list -> 1, 2
    /// list.add(3); // Append 3 to the list -> 1, 2, 3
    /// ```
    void add(const T &element) override;

    /// Add all elements from the given collection to the end of the list by copying them.
    ///
    /// ### Example
    /// ```c++
    /// auto list1 = Util::ArrayList<int>({1, 2, 3});
    /// auto list2 = Util::ArrayList<int>({4, 5, 6});
    ///
    /// list1.addAll(list2); // Append all elements from list2 to list1 -> 1, 2, 3, 4, 5, 6
    /// ```
    void addAll(const Collection<T> &collection) override;

    /// Add an element at the specified index by copying it.
    /// If the index is greater than the current length of the list, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>({1, 2, 3});
    /// list.addIndex(1, 4); // Insert 4 at index 1 -> 1, 4, 2, 3
    /// list.addIndex(0, 5); // Insert 5 at index 0 -> 5, 1, 4, 2, 3
    /// list.addIndex(6, 5); // Panic! (index out of bounds)
    /// ```
    void addIndex(size_t index, const T &element) override;

    /// Remove the first occurrence of the specified element from the list.
    /// Return `true` if the element was found and removed, `false` otherwise.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>({1, 2, 3, 2});
    /// list.remove(2); // Remove the first occurrence of 2 -> 1, 3, 2
    /// list.remove(4); // Return false, 4 is not in the list
    /// ```
    bool remove(const T &element) override;

    /// Remove all occurrences of the given collection from the list.
    /// Return `true` if any elements were removed, `false` otherwise.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>({1, 2, 3, 2, 4});
    /// list.removeAll(Util::ArrayList<int>({2, 4})); // Remove all occurrences of 2 and 4 -> 1, 3
    /// ```
    bool removeAll(const Collection<T> &collection) override;

    /// Remove the element at the specified index from the list and return a copy of it.
    /// If the index is greater than or equal to the current length of the list, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>({1, 2, 3, 4});
    /// list.removeIndex(2); // Remove the element at index 2 -> 1, 2, 4
    /// list.removeIndex(0); // Remove the element at index 0 -> 2, 4
    /// list.removeIndex(5); // Panic! (index out of bounds)
    /// ```
    T removeIndex(size_t index) override;

    /// Retain only the elements that are also contained in the given collection.
    /// Return `true` if any elements were removed, `false` otherwise.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>({1, 2, 3, 2, 3, 4, 5, 2, 4});
    /// list.retainAll(Util::ArrayList<int>({1, 3, 5})); /// Retain only 1, 3 and 5 -> 1, 3, 3, 5
    /// ```
    bool retainAll(const Collection<T> &collection) override;

    /// Check if the list contains the specified element.
    ///
    /// ### Example
    /// ```c++
    /// const auto list = Util::ArrayList<int>({1, 2, 3});
    /// const auto containsTwo = list.contains(2); // true
    /// const auto containsSix = list.contains(6); // false
    /// ```
    [[nodiscard]] bool contains(const T &element) const override;

    /// Check if the list contains all elements from the given collection.
    ///
    /// ### Example
    /// ```c++
    /// const auto list = Util::ArrayList<int>({1, 2, 3, 4, 5});
    /// const auto containsTwoThreeFour = list.containsAll(Util::ArrayList<int>({2, 3, 4})); // true
    /// const auto containsTwoThreeSix = list.containsAll(Util::ArrayList<int>({2, 3, 6})); // false
    /// ```
    [[nodiscard]] bool containsAll(const Collection<T> &collection) const override;

    /// Get the index of the first occurrence of the specified element in the list.
    /// If the element is not found, `SIZE_MAX` is returned.
    ///
    /// ### Example
    /// ```c++
    /// const auto list = Util::ArrayList<int>({1, 2, 3, 2, 4});
    /// const auto indexOfTwo = list.indexOf(2); // 1
    /// const auto indexOfFive = list.indexOf(5); // SIZE_MAX (not found)
    /// ```
    [[nodiscard]] size_t indexOf(const T &element) const override;

    /// Get the element at the specified index without removing it.
    /// The element is copied from the list and returned by value.
    /// If the index is greater than or equal to the current length of the list, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// const auto list = Util::ArrayList<int>({1, 2, 3, 4, 5});
    /// const auto element = list.get(2); // Get the element at index 2 -> 3
    /// const auto outOfBounds = list.get(5); // Panic! (index out of bounds)
    /// ```
    [[nodiscard]] T get(size_t index) const override;

    /// Overwrite the element at the specified index with the given element by copying it.
    /// If the index is greater than or equal to the current length of the list, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>({1, 2, 3, 4, 5});
    /// list.set(2, 8); // Set the element at index 2 to 8 -> 1, 2, 8, 4, 5
    /// list.set(5, 10); // Panic! (index out of bounds)
    /// ```
    void set(size_t index, const T &element) override;

    /// Check if the list is empty.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>();
    /// auto isEmpty = list.isEmpty(); // true
    ///
    /// list.add(1);
    /// isEmpty = list.isEmpty(); // false
    /// ```
    [[nodiscard]] bool isEmpty() const override;

    /// Clear the list by removing all elements.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>({1, 2, 3, 4, 5});
    /// list.clear(); // Remove all elements from the list
    /// const auto isEmpty = list.isEmpty(); // true
    /// ```
    void clear() override;

    /// Get the number of elements in the list.
    ///
    /// ### Example
    /// ```c++
    /// auto list = Util::ArrayList<int>({1, 2, 3, 4, 5});
    /// auto size = list.size(); // 5
    ///
    /// list.add(6);
    /// size = list.size(); // 6
    ///
    /// list.clear();
    /// size = list.size(); // 0
    /// ```
    [[nodiscard]] size_t size() const override;

    /// Convert the list to an array and return it.
    /// All elements are copied to the new array.
    ///
    /// ### Example
    /// ```c++
    /// const auto list = Util::ArrayList<int>({1, 2, 3, 4, 5});
    /// auto array = list.toArray(); // Convert the list to an array
    /// array[0] = 10; // Modify the first element of the array; The list remains unchanged
    /// ```
    [[nodiscard]] Array<T> toArray() const override;

    /// Create an iterator for the list, starting at the first element.
    /// This allows iteration over the elements of the list using a range-based for loop.
    Iterator<T> begin() const override;

    /// Create an iterator for the list, pointing to the end (one past the last element).
    /// This allows iteration over the elements of the list using a range-based for loop.
    Iterator<T> end() const override;

    /// Get the next element in the iteration based on the current element.
    IteratorElement<T> next(const IteratorElement<T> &element) const override;

private:

    /// Ensure that the internal array has enough capacity to hold at least `newCapacity` elements.
    /// This method is called automatically when adding elements to the list.
    void ensureCapacity(size_t newCapacity);

    T *elements = nullptr;
    size_t capacity = 0;
    size_t length = 0;
};

template <class T>
ArrayList<T>::ArrayList(const size_t capacity) : capacity(capacity) {
    elements = new T[capacity];
}

template<typename T>
ArrayList<T>::ArrayList(const Array<T> &elements) : ArrayList(elements.length()) {
    for(const auto &element : elements)  {
        add(element);
    }
}

template<typename T>
ArrayList<T>::ArrayList(const std::initializer_list<T> &list) : ArrayList(list.size()) {
    for (const auto &element : list) {
        add(element);
    }
}

template <class T>
ArrayList<T>::~ArrayList() {
    delete[] elements;
}

template <class T>
void ArrayList<T>::add(const T &element) {
    ensureCapacity(length + 1);
    elements[length++] = element;
}

template<typename T>
void ArrayList<T>::addAll(const Collection<T> &collection) {
    for (const auto &element : collection) {
        add(element);
    }
}

template <class T>
void ArrayList<T>::addIndex(const size_t index, const T &element) {
    if (index > length) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "ArrayList: Trying to add an element out of bounds!");
    }

    ensureCapacity(length + 1);

    for(size_t i = length; i > index; i--) {
        elements[i] = elements[i - 1];
    }

    elements[index] = element;
    length++;
}

template <class T>
bool ArrayList<T>::remove(const T &element) {
    const auto index = indexOf(element);

    if (index >= length) {
        return false;
    }

    removeIndex(index);
    return true;
}

template<typename T>
bool ArrayList<T>::removeAll(const Collection<T> &collection) {
    bool changed = false;
    for (const auto &element : collection) {
        while (remove(element)) {
            changed = true;
        }
    }

    return changed;
}

template <class T>
T ArrayList<T>::removeIndex(const size_t index) {
    if (index >= length) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "ArrayList: Trying to access an element out of bounds!");
    }

    T tmp = elements[index];
    const size_t numMoved = length - index - 1;

    if (numMoved != 0) {
        for(size_t i = 0; i < numMoved; i++) {
            elements[index + i] = elements[index + i + 1];
        }
    }

    length--;
    return tmp;
}

template<typename T>
bool ArrayList<T>::retainAll(const Collection<T> &collection) {
    bool changed = false;
    for (size_t i = 0; i < length;) {
        if (!collection.contains(elements[i])) {
            removeIndex(i);
            changed = true;
        } else {
            i++;
        }
    }

    return changed;
}

template <class T>
bool ArrayList<T>::contains(const T &element) const {
    return indexOf(element) < length;
}

template<typename T>
bool ArrayList<T>::containsAll(const Collection<T> &collection) const {
    for (const auto &element : collection) {
        if (!contains(element)) {
            return false;
        }
    }

    return true;
}

template <class T>
size_t ArrayList<T>::indexOf(const T &element) const {
    size_t index;
    for (index = 0; index < length && elements[index] != element; index++) {}

    return index == length ? List<T>::INVALID_INDEX : index;
}

template <class T>
T ArrayList<T>::get(const size_t index) const {
    if (index >= length) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "ArrayList: Trying to get an element out of bounds!");
    }

    return elements[index];
}

template <class T>
void ArrayList<T>::set(const size_t index, const T &element) {
    if (index >= length) {
        Panic::fire(Panic::OUT_OF_BOUNDS, "ArrayList: Trying to set an element out of bounds!");
    }

    elements[index] = element;
}

template <class T>
bool ArrayList<T>::isEmpty() const {
    return length == 0;
}

template <class T>
void ArrayList<T>::clear() {
    length = 0;
}

template <class T>
size_t ArrayList<T>::size() const {
    return length;
}

template <class T>
Array<T> ArrayList<T>::toArray() const {
    Array<T> array(length);

    for (size_t i = 0; i < length; i++) {
        array[i] = elements[i];
    }

    return array;
}

template<typename T>
Iterator<T> ArrayList<T>::begin() const {
    const auto element = IteratorElement<T>{elements, 0};
    return Iterator<T>(*this, element);
}

template<typename T>
Iterator<T> ArrayList<T>::end() const {
    const auto element = IteratorElement<T>{elements + length, length};
    return Iterator<T>(*this, element);
}

template<typename T>
IteratorElement<T> ArrayList<T>::next(const IteratorElement<T> &element) const {
    return IteratorElement<T>{element.data + 1, element.index + 1};
}

template <class T>
void ArrayList<T>::ensureCapacity(const size_t newCapacity) {
    if (capacity == 0) {
        capacity = 1;
        elements = new T[capacity];
    }

    if (newCapacity <= capacity) {
        return;
    }

    const size_t oldCapacity = capacity;
    while (capacity < newCapacity) {
        capacity *= 2;
    }

    T *tmp = elements;
    elements = new T[capacity];

    for (size_t i = 0; i < oldCapacity; i++) {
        elements[i] = tmp[i];
    }

    delete[] tmp;
}

}

#endif
