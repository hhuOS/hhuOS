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

#ifndef __ArrayList_include__
#define __ArrayList_include__

#include "lib/util/base/String.h"
#include "List.h"

namespace Util {

/**
 * An implementation of the List interface backed by a dynamically growing array of elements.
 *
 * @author Filip Krakowski
 */
template <typename T>
class ArrayList : public List<T> {

public:

    ArrayList() noexcept;

    explicit ArrayList(uint32_t capacity) noexcept;

    explicit ArrayList(Util::Array<T> elements) noexcept;

    ArrayList(const ArrayList<T> &other) = delete;

    ArrayList<T> &operator=(const ArrayList<T> &other) = delete;

    ~ArrayList();

    bool add(const T &element) override;

    void add(uint32_t index, const T &element) override;

    bool addAll(const Collection<T> &other) override;

    [[nodiscard]] T get(uint32_t index) const override;

    void set(uint32_t index, const T &element) override;

    bool remove(const T &element) override;

    bool removeAll(const Collection<T> &other) override;

    T removeIndex(uint32_t index) override;

    [[nodiscard]] bool contains(const T &element) const override;

    [[nodiscard]] bool containsAll(const Collection<T> &other) const override;

    [[nodiscard]] uint32_t indexOf(const T &element) const override;

    [[nodiscard]] bool isEmpty() const override;

    void clear() override;

    Iterator<T> begin() const override;

    Iterator<T> end() const override;

    [[nodiscard]] uint32_t size() const override;

    [[nodiscard]] Array<T> toArray() const override;

private:

    void ensureCapacity(uint32_t newCapacity) override;

    T *elements = nullptr;
    uint32_t capacity = 0;
    uint32_t length = 0;

    static const constexpr uint32_t DEFAULT_CAPACITY = 8;
};

template <class T>
ArrayList<T>::ArrayList() noexcept {
    this->length = 0;
    this->capacity = 0;
    this->elements = nullptr;
}

template <class T>
ArrayList<T>::ArrayList(uint32_t capacity) noexcept {
    this->length = 0;
    this->capacity = capacity;
    this->elements = new T[capacity];
}

template<typename T>
ArrayList<T>::ArrayList(Array<T> elements) noexcept : ArrayList(elements.length()) {
    for(const auto &element : elements)  {
        add(element);
    }
}

template <class T>
ArrayList<T>::~ArrayList() {
    delete[] elements;
}

template <class T>
T ArrayList<T>::get(uint32_t index) const {
    if (index >= length) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "ArrayList: Trying to access an element out of bounds!");
    }

    return elements[index];
}

template <class T>
bool ArrayList<T>::add(const T &element) {
    ensureCapacity(length + 1);
    elements[length] = element;
    length++;

    return true;
}

template <class T>
bool ArrayList<T>::addAll(const Collection<T> &other) {
    for (const T &element : other) {
        add(element);
    }

    return true;
}

template <class T>
void ArrayList<T>::add(uint32_t index, const T &element) {
    if (index > length) {
        return;
    }

    ensureCapacity(length + 1);

    for(uint32_t i = length - 1; i > index; i--) {
        elements[i] = elements[i - 1];
    }

    elements[index] = element;
    length++;
}

template <class T>
bool ArrayList<T>::remove(const T &element) {
    uint32_t index = indexOf(element);

    if (index >= length) {
        return false;
    }

    removeIndex(index);
    return true;
}

template <class T>
T ArrayList<T>::removeIndex(uint32_t index) {
    if (index >= length) {
        Exception::throwException(Exception::OUT_OF_BOUNDS, "ArrayList: Trying to access an element out of bounds!");
    }

    T tmp = elements[index];
    uint32_t numMoved = length - index - 1;

    if (numMoved != 0) {
        for(uint32_t i = 0; i < numMoved; i++) {
            elements[index + i] = elements[index + i + 1];
        }
    }

    length--;
    return tmp;
}

template <class T>
bool ArrayList<T>::removeAll(const Collection<T> &other) {
    bool changed = false;

    for (const T &element : other) {
        if (remove(element)) {
            changed = true;
        }
    }

    return changed;
}

template <class T>
uint32_t ArrayList<T>::indexOf(const T &element) const {
    uint32_t index;
    for (index = 0; index < length && elements[index] != element; index++) {}

    return index == length ? UINT32_MAX : index;
}

template <class T>
bool ArrayList<T>::contains(const T &element) const {
    return indexOf(element) < length;
}

template <class T>
bool ArrayList<T>::containsAll(const Collection<T> &other) const {
    for (const T &element : other) {
        if (!contains(element)) {
            return false;
        }
    }

    return true;
}

template <class T>
void ArrayList<T>::clear() {
    length = 0;
}

template <class T>
uint32_t ArrayList<T>::size() const {
    return length;
}

template <class T>
void ArrayList<T>::ensureCapacity(uint32_t newCapacity) {
    if (capacity == 0) {
        capacity = DEFAULT_CAPACITY;
        elements = new T[capacity];
    }

    if (newCapacity <= capacity) {
        return;
    }

    uint32_t oldCapacity = capacity;
    while (capacity < newCapacity) {
        capacity *= 2;
    }

    T* tmp = elements;
    elements = new T[capacity];

    for (uint32_t i = 0; i < oldCapacity; i++) {
        elements[i] = tmp[i];
    }

    delete[] tmp;
}

template <class T>
Iterator<T> ArrayList<T>::begin() const {
    return Iterator<T>(toArray(), 0);
}

template <class T>
Iterator<T> ArrayList<T>::end() const {
    return Iterator<T>(toArray(), length);
}

template <class T>
bool ArrayList<T>::isEmpty() const {
    return length == 0;
}

template <class T>
void ArrayList<T>::set(uint32_t index, const T &element) {
    if (index >= length) {
        return;
    }

    elements[index] = element;
}

template <class T>
Array<T> ArrayList<T>::toArray() const {
    Array<T> array(length);

    for (uint32_t i = 0; i < length; i++) {
        array[i] = elements[i];
    }

    return array;
}

}

#endif
