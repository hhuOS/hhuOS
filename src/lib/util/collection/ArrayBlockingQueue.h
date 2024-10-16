/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_ARRAYQUEUE_H
#define HHUOS_ARRAYQUEUE_H

#include "Queue.h"
#include "lib/util/async/Thread.h"

namespace Util {

template <typename T>
class ArrayBlockingQueue : public Queue<T> {

public:

    ArrayBlockingQueue();

    explicit ArrayBlockingQueue(uint32_t capacity);

    ~ArrayBlockingQueue();

    ArrayBlockingQueue(const ArrayBlockingQueue<T> &other) = delete;

    ArrayBlockingQueue<T> &operator=(const ArrayBlockingQueue<T> &other) = delete;

    bool offer(const T &element) override;

    T poll() override;

    T peek() override;

    bool add(const T &element) override;

    bool addAll(const Collection<T> &other) override;

    bool remove(const T &element) override;

    bool removeAll(const Collection<T> &other) override;

    [[nodiscard]] bool contains(const T &element) const override;

    [[nodiscard]] bool containsAll(const Collection<T> &other) const override;

    [[nodiscard]] bool isEmpty() const override;

    void clear() override;

    Iterator<T> begin() const override;

    Iterator<T> end() const override;

    [[nodiscard]] uint32_t size() const override;

    [[nodiscard]] Array<T> toArray() const override;

private:

    T *elements;
    uint32_t capacity;

    uint32_t head = 0;
    uint32_t tail = -1;
    uint32_t length = 0;

    static const uint32_t DEFAULT_CAPACITY = 16;
};

template<class T>
ArrayBlockingQueue<T>::ArrayBlockingQueue() : elements(new T[DEFAULT_CAPACITY]), capacity(DEFAULT_CAPACITY) {}

template<class T>
ArrayBlockingQueue<T>::ArrayBlockingQueue(uint32_t capacity) : elements(new T[capacity]), capacity(capacity) {}

template<typename T>
ArrayBlockingQueue<T>::~ArrayBlockingQueue() {
    delete[] elements;
}

template<class T>
bool ArrayBlockingQueue<T>::offer(const T &element) {
    if (length == capacity) {
        return false;
    }

    tail = (tail + 1) % capacity;
    elements[tail] = element;
    length++;

    return true;
}

template<class T>
T ArrayBlockingQueue<T>::poll() {
    while (length == 0) {
        Util::Async::Thread::yield();
    }

    auto &element = elements[head];
    head = (head + 1) % capacity;
    length--;

    return element;
}

template<class T>
T ArrayBlockingQueue<T>::peek() {
    while (length == 0) {
        Util::Async::Thread::yield();
    }

    return elements[head];
}

template<class T>
bool ArrayBlockingQueue<T>::add(const T &element) {
    while (!offer(element)) {
        Util::Async::Thread::yield();
    }

    return true;
}

template<class T>
bool ArrayBlockingQueue<T>::addAll(const Collection<T> &other) {
    for (const auto &element : other) {
        add(element);
    }

    return true;
}
template<class T>
bool ArrayBlockingQueue<T>::remove([[maybe_unused]] const T &element) {
    Exception::throwException(Exception::UNSUPPORTED_OPERATION, "ArrayBlockingQueue: Remove a specific element is not supported!");
}
template<class T>
bool ArrayBlockingQueue<T>::removeAll([[maybe_unused]] const Collection<T> &other) {
    Exception::throwException(Exception::UNSUPPORTED_OPERATION, "ArrayBlockingQueue: Remove a collection of specific elements is not supported!");
}

template<class T>
bool ArrayBlockingQueue<T>::contains(const T &element) const {
    for (uint32_t i = head; i < tail; i++) {
        if (elements[i] == element) {
            return true;
        }
    }

    return false;
}

template<class T>
bool ArrayBlockingQueue<T>::containsAll(const Collection<T> &other) const {
    for (const T &element : other) {
        if (!contains(element)) {
            return false;
        }
    }

    return true;
}

template<class T>
bool ArrayBlockingQueue<T>::isEmpty() const {
    return length == 0;
}

template<class T>
void ArrayBlockingQueue<T>::clear() {
    head = 0;
    tail = -1;
    length = 0;
}

template<class T>
Iterator<T> ArrayBlockingQueue<T>::begin() const {
    return Iterator<T>(toArray(), 0);
}

template<class T>
Iterator<T> ArrayBlockingQueue<T>::end() const {
    return Iterator<T>(toArray(), length);
}

template<class T>
uint32_t ArrayBlockingQueue<T>::size() const {
    return length;
}

template<class T>
Array<T> ArrayBlockingQueue<T>::toArray() const {
    Array<T> array(length);
    for (uint32_t i = head; i < tail; i++) {
        array[i] = elements[i];
    }

    return array;
}

}

#endif
