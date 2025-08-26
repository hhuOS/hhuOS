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

#ifndef HHUOS_LIB_UTIL_ARRAY_LIST_QUEUE_H
#define HHUOS_LIB_UTIL_ARRAY_LIST_QUEUE_H

#include <stddef.h>

#include "async/Thread.h"
#include "collection/ArrayList.h"
#include "collection/Queue.h"

namespace Util {

/// A queue implementation based on an `ArrayList` to store the elements.
/// It works by appending elements to the end of the list and removing them from the front.
///
/// ## Example
/// ```c++
/// auto queue = Util::ArrayListQueue<int>();
/// queue.offer(1); // Add 1 to the queue -> 1
/// queue.offer(2); // Add 2 to the queue -> 1, 2
/// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
///
/// auto first = queue.poll(); // Retrieve the first element -> first = 1
/// queue.offer(first); // Add it back to the queue -> 2, 3, 1
///
/// first = queue.peek(); // Peek at the first element without removing it -> first = 2
///
/// for (const auto &element : queue) {
///     Util::System::out << element << " "; // Prints: 2 3 1
/// }
///
/// Util::System::out << Util::Io::PrintStream::endl << Util::Io::PrintStream::flush;
/// ```
template <typename T>
class ArrayListQueue final : public Queue<T> {

public:
    /// Create a new empty ArrayListQueue instance with a default capacity of 0.
    ArrayListQueue() = default;

    /// Create a new ArrayListQueue instance with the given initial capacity.
    explicit ArrayListQueue(size_t capacity);

    /// ArrayListQueue is not copyable, so the copy constructor is deleted.
    ArrayListQueue(const ArrayListQueue<T> &other) = delete;

    /// ArrayListQueue is not copyable, so the assignment operator is deleted.
    ArrayListQueue<T> &operator=(const ArrayListQueue<T> &other) = delete;

    /// The ArrayListQueue destructor is trivial, as this class only contains an ArrayList.
    ~ArrayListQueue() override = default;

    /// Offer an element to the queue, adding it to the end of the queue by copying it.
    /// This implementation will always succeed, as the ArrayList can grow dynamically.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    /// ```
    bool offer(const T &element) override;

    /// Add an element to end of the queue by copying it.
    /// In this implementation, this method is equivalent to `offer()`.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.add(1); // Add 1 to the queue -> 1
    /// queue.add(2); // Add 2 to the queue -> 1, 2
    /// queue.add(3); // Add 3 to the queue -> 1, 2, 3
    /// ```
    void add(const T &element) override;

    /// Add all elements from the given collection to the end of the queue by copying them.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    ///
    /// auto list = Util::ArrayList<int>({4, 5, 6});
    /// queue.addAll(list); // Add all elements from the list to the queue -> 1, 2, 3, 4, 5, 6
    /// ```
    void addAll(const Collection<T> &collection) override;

    /// Remove the first element from the queue and return a copy of it.
    /// If the queue is empty, this method will block until an element is available.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    ///
    /// auto element = queue.poll(); // element = 1
    /// element = queue.poll(); // element = 2
    /// element = queue.poll(); // element = 3
    /// element = queue.poll(); // This will block until an element is available, as the queue is now empty
    /// ```
    T poll() override;

    /// Peek at the first element in the queue and return a copy of it without removing it.
    /// If the queue is empty, this method will block until an element is available.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    ///
    /// const auto first = queue.peek(); // first = 1
    /// ```
    T peek() const override;

    /// Get the element at the specified index in the queue without removing it.
    /// The element is copied from the list and returned by value.
    /// If the index is greater than or equal to the current length of the queue, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    ///
    /// const auto element = queue.get(1); // element = 2
    /// ```
    T get(size_t index);

    /// Check if the queue contains the specified element.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    ///
    /// const auto containsTwo = queue.contains(2); // true
    /// const auto containsFour = queue.contains(4); // false
    /// ```
    bool contains(const T &element) const override;

    /// Check if the queue contains all elements from the given collection.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    /// query.offer(4); // Add 4 to the queue -> 1, 2, 3, 4
    /// query.offer(5); // Add 5 to the queue -> 1, 2, 3, 4, 5
    ///
    /// const auto containsTwoThreeFour = queue.containsAll(Util::ArrayList<int>({2, 3, 4})); // true
    /// const auto containsTwoThreeSix = queue.containsAll(Util::ArrayList<int>({2, 3, 6})); // false
    /// ```
    [[nodiscard]] bool containsAll(const Collection<T> &collection) const override;

    /// Remove the first occurrence of the specified element from the list.
    /// Return `true` if the element was found and removed, `false` otherwise.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    /// queue.offer(2); // Add another 2 to the queue -> 1, 2, 3, 2
    ///
    /// queue.remove(2); // Remove 2 from the queue -> 1, 3, 2
    /// queue.remove(4); // Return false, as 4 is not in the queue
    /// ```
    bool remove(const T &element) override;

    /// Remove all occurrences of the given collection from the queue.
    /// Return `true` if any elements were removed, `false` otherwise.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    /// queue.offer(2); // Add another 2 to the queue -> 1, 2, 3, 2
    /// queue.offer(4); // Add 4 to the queue -> 1, 2, 3, 2, 4
    ///
    /// queue.removeAll(Util::ArrayList<int>({2, 4})); // Remove all occurrences of 2 and 4 -> 1, 3
    /// ```
    bool removeAll(const Collection<T> &collection) override;

    /// Retain only the elements that are also contained in the given collection.
    /// Return `true` if any elements were removed, `false` otherwise.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    /// queue.offer(2); // Add another 2 to the queue -> 1, 2, 3, 2
    /// queue.offer(4); // Add 4 to the queue -> 1, 2, 3, 2, 4
    ///
    /// queue.retainAll(Util::ArrayList<int>({1, 3, 5})); // Retain only 1, 3 and 5 -> 1, 3
    /// ```
    bool retainAll(const Collection<T> &collection) override;

    /// Check if the queue is empty.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// auto isEmpty = queue.isEmpty(); // true
    ///
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// isEmpty = queue.isEmpty(); // false
    /// ```
    [[nodiscard]] bool isEmpty() const override;

    /// Clear the queue, removing all elements.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    ///
    /// queue.clear(); // Remove all elements from the queue
    /// const auto isEmpty = queue.isEmpty(); // true
    /// ```
    void clear() override;

    /// Get the number of elements in the queue.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// auto size = queue.size(); // 0
    ///
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    /// size = queue.size(); // 3
    ///
    /// queue.clear(); // Remove all elements from the queue
    /// size = queue.size(); // 0
    /// ```
    [[nodiscard]] size_t size() const override;

    /// Convert the queue to an array and return it.
    /// All elements are copied to the new array.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayListQueue<int>();
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    ///
    /// auto array = queue.toArray(); // Convert the queue to an array
    /// array[0] = 10; // Modify the first element of the array; The queue remains unchanged
    /// ```
    [[nodiscard]] Array<T> toArray() const override;

    /// Create an iterator for the queue, starting at the first element.
    /// This allows iteration over the elements of the queue using a range-based for loop.
    Iterator<T> begin() const override;

    /// Create an iterator for the queue, pointing to the end (one past the last element).
    /// This allows iteration over the elements of the queue using a range-based for loop.
    Iterator<T> end() const override;

    /// Get the next element in the iteration based on the current element.
    IteratorElement<T> next(const IteratorElement<T> &element) const override;

private:

    ArrayList<T> elements;
};

template<class T>
ArrayListQueue<T>::ArrayListQueue(const size_t capacity) : elements(capacity) {}

template<class T>
bool ArrayListQueue<T>::offer(const T &element) {
    elements.add(element);
    return true;
}

template<class T>
T ArrayListQueue<T>::poll() {
    while (elements.isEmpty()) {
        Async::Thread::yield();
    }
    
    return elements.removeIndex(0);
}

template<typename T>
T ArrayListQueue<T>::peek() const {
    while (elements.isEmpty()) {
        Async::Thread::yield();
    }
    
    return elements.get(0);
}

template<typename T>
T ArrayListQueue<T>::get(const size_t index) {
    return elements.get(index);
}

template<typename T>
bool ArrayListQueue<T>::contains(const T &element) const {
    return elements.contains(element);
}

template<typename T>
bool ArrayListQueue<T>::containsAll(const Collection<T> &collection) const {
    return elements.containsAll(collection);
}

template<typename T>
bool ArrayListQueue<T>::removeAll(const Collection<T> &collection) {
    return elements.removeAll(collection);
}

template<typename T>
bool ArrayListQueue<T>::retainAll(const Collection<T> &collection) {
    return elements.retainAll(collection);
}

template<typename T>
void ArrayListQueue<T>::addAll(const Collection<T> &collection) {
    elements.addAll(collection);
}

template<typename T>
bool ArrayListQueue<T>::remove(const T &element) {
    return elements.remove(element);
}

template<class T>
void ArrayListQueue<T>::add(const T &element) {
    elements.add(element);
}

template<class T>
bool ArrayListQueue<T>::isEmpty() const {
    return elements.isEmpty();
}

template<class T>
void ArrayListQueue<T>::clear() {
    elements.clear();
}

template<class T>
size_t ArrayListQueue<T>::size() const {
    return elements.size();
}

template<class T>
Array<T> ArrayListQueue<T>::toArray() const {
    return elements.toArray();
}

template<typename T>
Iterator<T> ArrayListQueue<T>::begin() const {
    return elements.begin();
}

template<typename T>
Iterator<T> ArrayListQueue<T>::end() const {
    return elements.end();
}

template<typename T>
IteratorElement<T> ArrayListQueue<T>::next(const IteratorElement<T> &element) const {
    return elements.next(element);
}

}

#endif
