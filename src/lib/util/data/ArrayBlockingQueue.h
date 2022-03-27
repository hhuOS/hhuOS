/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef __BlockingQueue_include__
#define __BlockingQueue_include__

#include "Queue.h"
#include "ArrayList.h"

namespace Util::Data {

/**
 * An implementation of the Queue interface.
 *
 * @author Filip Krakowski
 */
template <typename T>
class ArrayBlockingQueue : public Queue<T> {

public:

    ArrayBlockingQueue();

    explicit ArrayBlockingQueue(uint32_t capacity);

    ~ArrayBlockingQueue();

    ArrayBlockingQueue(const ArrayBlockingQueue<T> &other) = delete;

    ArrayBlockingQueue<T> &operator=(const ArrayBlockingQueue<T> &other) = delete;

    void push(const T &element) override;

    T pop() override;

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

    List<T> &elements;

    bool deleteList;

    static const uint32_t DEFAULT_CAPACITY = 16;

};

template<class T>
ArrayBlockingQueue<T>::ArrayBlockingQueue() : elements(*new ArrayList<T>(DEFAULT_CAPACITY)), deleteList(true) {}

template<class T>
ArrayBlockingQueue<T>::ArrayBlockingQueue(uint32_t capacity) : elements(*new ArrayList<T>(DEFAULT_CAPACITY)), deleteList(true) {}

template<typename T>
ArrayBlockingQueue<T>::~ArrayBlockingQueue() {
    if(deleteList) {
        delete &elements;
    }
}

template<class T>
void ArrayBlockingQueue<T>::push(const T &element) {
    elements.add(element);
}

template<class T>
T ArrayBlockingQueue<T>::pop() {
    while (isEmpty()) {}
    return elements.removeIndex((uint32_t) 0);
}

template<class T>
bool ArrayBlockingQueue<T>::add(const T &element) {
    return elements.add(element);
}

template<class T>
bool ArrayBlockingQueue<T>::addAll(const Collection<T> &other) {
    return elements.addAll(other);
}

template<class T>
bool ArrayBlockingQueue<T>::remove(const T &element) {
    return elements.remove(element);
}

template<class T>
bool ArrayBlockingQueue<T>::removeAll(const Collection<T> &other) {
    return elements.removeAll(other);
}

template<class T>
bool ArrayBlockingQueue<T>::contains(const T &element) const {
    return elements.contains(element);
}

template<class T>
bool ArrayBlockingQueue<T>::containsAll(const Collection<T> &other) const {
    return elements.containsAll(other);
}

template<class T>
bool ArrayBlockingQueue<T>::isEmpty() const {
    return elements.isEmpty();
}

template<class T>
void ArrayBlockingQueue<T>::clear() {
    elements.clear();
}

template<class T>
Iterator<T> ArrayBlockingQueue<T>::begin() const {
    return elements.begin();
}

template<class T>
Iterator<T> ArrayBlockingQueue<T>::end() const {
    return elements.end();
}

template<class T>
uint32_t ArrayBlockingQueue<T>::size() const {
    return elements.size();
}

template<class T>
Array<T> ArrayBlockingQueue<T>::toArray() const {
    return elements.toArray();
}

}

#endif
