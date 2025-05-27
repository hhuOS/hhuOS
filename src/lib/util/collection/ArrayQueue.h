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

#ifndef HHUOS_LIB_UTIL_ARRAY_QUEUE_H
#define HHUOS_LIB_UTIL_ARRAY_QUEUE_H

#include "async/Thread.h"
#include "collection/Queue.h"

namespace Util {

/// A bounded blocking queue implementation based on a fixed-size array, implementing only basic queue operations.
/// It is thread-safe for a single producer and a single consumer scenario.
/// This is for example useful for interrupt handling, where data needs to be queued and processed in a FIFO manner,
/// without using any locking mechanisms.
///
/// ## Example
/// ```
/// auto queue = Util::ArrayQueue<int>(4); // Create a queue with a capacity of 4 elements
/// queue.add(1); // Add 1 to the queue -> 1
/// queue.add(2); // Add 2 to the queue -> 1, 2
/// queue.add(3); // Add 3 to the queue -> 1, 2, 3
/// queue.add(4); // Add 4 to the queue -> 1, 2, 3, 4
///
/// auto first = queue.poll(); // Retrieve the first element -> first = 1
/// queue.offer(first); // Add it back to the queue -> 2, 3, 4, 1
///
/// first = queue.peek(); // Peek at the first element without removing it -> first = 2
///
/// while (!queue.isEmpty()) {
///     auto element = queue.poll(); // Remove an element from the queue
///     printf("%d ", element); // Prints: 2 3 4 1
/// }
/// ```
template <typename T>
class ArrayQueue final : public Queue<T> {

public:
    /// Create a new empty ArrayQueue instance with the given capacity.
    explicit ArrayQueue(size_t capacity);

    /// Delete the ArrayQueue instance and free the allocated memory.
    ~ArrayQueue() override;

    /// ArrayQueue is not copyable, so the copy constructor is deleted.
    ArrayQueue(const ArrayQueue &other) = delete;

    /// ArrayQueue is not assignable, so the assignment operator is deleted.
    ArrayQueue &operator=(const ArrayQueue &other) = delete;

    /// Offer an element to the queue, adding it to the end of the queue by copying it.
    /// If the queue is full, the method returns false without blocking.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayQueue<int>(4);
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    /// queue.offer(4); // Add 4 to the queue -> 1, 2, 3, 4
    /// queue.offer(5); // Returns false, as the queue is full -> 1, 2, 3, 4
    /// ```
    bool offer(const T &element) override;

    /// Add an element to end of the queue by copying it.
    /// If the queue is full, this method will block until space is available.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayQueue<int>(4);
    /// queue.add(1); // Add 1 to the queue -> 1
    /// queue.add(2); // Add 2 to the queue -> 1, 2
    /// queue.add(3); // Add 3 to the queue -> 1, 2, 3
    /// queue.add(4); // Add 4 to the queue -> 1, 2, 3, 4
    /// queue.add(5); // This will block until space is available, as the queue is full
    /// ```
    void add(const T &element) override;

    /// Add all elements from the given collection to the end of the queue by copying them.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayQueue<int>(4);
    /// auto list1 = Util::ArrayList<int>({1, 2, 3});
    /// auto list2 = Util::ArrayList<int>({4, 5, 6});
    ///
    /// queue.addAll(list1); // Add 1, 2, 3 to the queue -> 1, 2, 3
    /// queue.addAll(list2); // The 4 will be added to the queue, but 5 and 6 will block until space is available
    /// ```
    void addAll(const Collection<T> &collection) override;

    /// Remove the first element from the queue and return a copy of it.
    /// If the queue is empty, this method will block until an element is available.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayQueue<int>(4);
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    /// queue.offer(4); // Add 4 to the queue -> 1, 2, 3, 4
    ///
    /// auto element = queue.poll(); // element = 1
    /// element = queue.poll(); // element = 2
    /// element = queue.poll(); // element = 3
    /// element = queue.poll(); // element = 4
    /// element = queue.poll(); // This will block until an element is available, as the queue is now empty
    /// ```
    T poll() override;

    /// Peek at the first element in the queue and return a copy of it without removing it.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayQueue<int>(4);
    /// queue.offer(1); // Add 1 to the queue -> 1
    /// queue.offer(2); // Add 2 to the queue -> 1, 2
    /// queue.offer(3); // Add 3 to the queue -> 1, 2, 3
    ///
    /// const auto first = queue.peek(); // first = 1
    /// ```
    T peek() const override;

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    bool remove(const T &element) override;

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    bool removeAll(const Collection<T> &collection) override;

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    bool retainAll(const Collection<T> &collection) override;

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    bool contains(const T &element) const override;

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    [[nodiscard]] bool containsAll(const Collection<T> &collection) const override;

    /// Check if the queue is empty.
    ///
    /// ### Example
    /// ```c++
    /// auto queue = Util::ArrayQueue<int>(4);
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
    /// auto queue = Util::ArrayQueue<int>(4);
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
    /// auto queue = Util::ArrayQueue<int>(4);
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

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    [[nodiscard]] Array<T> toArray() const override;

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    Iterator<T> begin() const override;

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    Iterator<T> end() const override;

    /// Not supported in ArrayQueue.
    /// Calling this method will fire a panic.
    IteratorElement<T> next(const IteratorElement<T> &element) const override;

private:

    T *elements;
    size_t capacity;

    size_t head = 0;
    size_t tail = -1;
    size_t length = 0;
};

template<class T>
ArrayQueue<T>::ArrayQueue(const size_t capacity) : elements(new T[capacity]), capacity(capacity) {}

template<typename T>
ArrayQueue<T>::~ArrayQueue() {
    delete[] elements;
}

template<class T>
bool ArrayQueue<T>::offer(const T &element) {
    if (length == capacity) {
        return false;
    }

    tail = (tail + 1) % capacity;
    elements[tail] = element;
    length++;

    return true;
}

template<class T>
T ArrayQueue<T>::poll() {
    while (length == 0) {
        Async::Thread::yield();
    }

    const auto &element = elements[head];
    head = (head + 1) % capacity;
    length--;

    return element;
}

template<class T>
T ArrayQueue<T>::peek() const {
    while (length == 0) {
        Async::Thread::yield();
    }

    return elements[head];
}

template<class T>
void ArrayQueue<T>::add(const T &element) {
    while (!offer(element)) {
        Async::Thread::yield();
    }
}

template<typename T>
void ArrayQueue<T>::addAll(const Collection<T> &collection) {
    for (const auto &element : collection) {
        add(element);
    }
}

template<typename T>
bool ArrayQueue<T>::remove([[maybe_unused]] const T &element) {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: remove() is not supported!");
}

template<typename T>
bool ArrayQueue<T>::removeAll([[maybe_unused]] const Collection<T> &collection) {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: removeAll() is not supported!");
}

template<typename T>
bool ArrayQueue<T>::retainAll([[maybe_unused]] const Collection<T> &collection) {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: retainAll() is not supported!");
}

template<typename T>
bool ArrayQueue<T>::contains([[maybe_unused]] const T &element) const {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: contains() is not supported!");
}

template<typename T>
bool ArrayQueue<T>::containsAll([[maybe_unused]] const Collection<T> &collection) const {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: containsAll() is not supported!");
}

template<class T>
bool ArrayQueue<T>::isEmpty() const {
    return length == 0;
}

template<class T>
void ArrayQueue<T>::clear() {
    head = 0;
    tail = -1;
    length = 0;
}

template<class T>
size_t ArrayQueue<T>::size() const {
    return length;
}

template<class T>
Array<T> ArrayQueue<T>::toArray() const {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: toArray() is not supported!");
}

template<typename T>
Iterator<T> ArrayQueue<T>::begin() const {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: Iterating is not supported!");
}

template<typename T>
Iterator<T> ArrayQueue<T>::end() const {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: Iterating is not supported!");
}

template<typename T>
IteratorElement<T> ArrayQueue<T>::next([[maybe_unused]] const IteratorElement<T> &element) const {
    Panic::fire(Panic::UNSUPPORTED_OPERATION,"BoundedBlockingQueue: Iterating is not supported!");
}

}

#endif
