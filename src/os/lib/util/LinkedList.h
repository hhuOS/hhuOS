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

#ifndef __LinkedList_include__
#define __LinkedList_include__

#include <cstdint>
#include <lib/String.h>
#include "List.h"
#include "LinkedBlockingQueue.h"


namespace Util {

template<typename T>
class LinkedList : public List<T>, public LinkedBlockingQueue<T> {

public:

    LinkedList() noexcept = default;

    LinkedList(const LinkedList<T> &other) = delete;

    LinkedList<T> &operator=(const LinkedList<T> &other) = delete;

    ~LinkedList();

    bool add(const T &element) override;

    void add(uint32_t index, const T &element) override;

    bool addAll(const Collection<T> &other) override;

    T get(uint32_t index) const override;

    void set(uint32_t index, const T &element) override;

    bool remove(const T &element) override;

    bool removeAll(const Collection<T> &other) override;

    T remove(uint32_t index) override;

    bool contains(const T &element) const override;

    bool containsAll(const Collection<T> &other) const override;

    uint32_t indexOf(const T &element) const override;

    bool isEmpty() const override;

    void clear() override;

    Iterator<T> begin() const override;

    Iterator<T> end() const override;

    uint32_t size() const override;

    Array<T> toArray() const override;

protected:

    void ensureCapacity(uint32_t newCapacity);

private:

    typedef typename LinkedBlockingQueue<T>::Node Node;
};

template<class T>
LinkedList<T>::~LinkedList() {
    LinkedBlockingQueue<T>::clear();
}

template<typename T>
bool LinkedList<T>::add(const T &element) {
    return LinkedBlockingQueue<T>::add(element);
}

template<typename T>
void LinkedList<T>::add(uint32_t index, const T &element) {
    if (index > LinkedBlockingQueue<T>::length || LinkedBlockingQueue<T>::head == nullptr) {
        LinkedBlockingQueue<T>::push(element);
    }

    Node *node = new Node(element);

    Node *current = LinkedBlockingQueue<T>::head;

    for(uint32_t i = 0; i < LinkedBlockingQueue<T>::length; i++) {
        if(i == index) {
            node->next = current->next;
            current->next = node;

            LinkedBlockingQueue<T>::length++;
            return;
        }

        current = current->next;
    }
}

template<typename T>
bool LinkedList<T>::addAll(const Collection <T> &other) {
    return LinkedBlockingQueue<T>::addAll(other);
}

template<class T>
T LinkedList<T>::remove(uint32_t index) {
    if(index == 0) {
        return LinkedBlockingQueue<T>::pop();
    }

    Node *pred = LinkedBlockingQueue<T>::head;
    Node *current = LinkedBlockingQueue<T>::head;

    for(uint32_t i = 0; i < LinkedBlockingQueue<T>::length; i++) {
        if(i == index) {
            pred->next = current->next;

            if(pred->next == nullptr) {
                LinkedBlockingQueue<T>::tail = pred;
            }

            LinkedBlockingQueue<T>::length--;

            T ret = current->element;
            delete current;

            return ret;
        }

        pred = current;
        current = current->next;
    }


    const char *errorMessage = String::format(
            "LinkedList: Trying to access an element at index %u, but length is %u!", index, LinkedBlockingQueue<T>::length);

    Cpu::throwException(Cpu::Exception::OUT_OF_BOUNDS, errorMessage);

    return current->element;
}

template<typename T>
bool LinkedList<T>::remove(const T &element) {
    return LinkedBlockingQueue<T>::remove(element);
}

template<typename T>
bool LinkedList<T>::removeAll(const Collection <T> &other) {
    return LinkedBlockingQueue<T>::removeAll(other);
}

template<typename T>
bool LinkedList<T>::contains(const T &element) const {
    return LinkedBlockingQueue<T>::contains(element);
}

template<typename T>
bool LinkedList<T>::containsAll(const Collection <T> &other) const {
    return LinkedBlockingQueue<T>::containsAll(other);
}

template<typename T>
void LinkedList<T>::set(uint32_t index, const T &element) {
    if(index >= LinkedBlockingQueue<T>::length) {
        return;
    }

    Node *current = LinkedBlockingQueue<T>::head;

    for(uint32_t i = 0; i < LinkedBlockingQueue<T>::length; i++) {
        if(i == index) {
            current->element = element;

            return;
        }

        current = current->next;
    }
}

template<class T>
uint32_t LinkedList<T>::indexOf(const T &element) const {
    Node *current = LinkedBlockingQueue<T>::head;
    uint32_t ret = 0;

    while (current->element != element) {
        if (current->next == nullptr) {
            return UINT32_MAX;
        }

        current = current->next;
        ret++;
    }

    return ret;
}
template<class T>
T LinkedList<T>::get(uint32_t index) const {
    if (index >= LinkedBlockingQueue<T>::length) {
        const char *errorMessage = String::format(
                "LinkedList: Trying to access an element at index %u, but length is %u!", index, LinkedBlockingQueue<T>::length);

        Cpu::throwException(Cpu::Exception::OUT_OF_BOUNDS, errorMessage);
    }

    Node *current = LinkedBlockingQueue<T>::head;

    for (uint32_t i = 0; i < index; i++) {
        current = current->next;
    }

    return current->element;
}

template<typename T>
void LinkedList<T>::ensureCapacity(uint32_t newCapacity) {

}

template<typename T>
bool LinkedList<T>::isEmpty() const {
    return LinkedBlockingQueue<T>::isEmpty();
}

template<typename T>
void LinkedList<T>::clear() {
    LinkedBlockingQueue<T>::clear();
}

template<typename T>
Iterator <T> LinkedList<T>::begin() const {
    return LinkedBlockingQueue<T>::begin();
}

template<typename T>
Iterator <T> LinkedList<T>::end() const {
    return LinkedBlockingQueue<T>::end();
}

template<typename T>
uint32_t LinkedList<T>::size() const {
    return LinkedBlockingQueue<T>::size();
}

template<typename T>
Array <T> LinkedList<T>::toArray() const {
    return LinkedBlockingQueue<T>::toArray();
}

}

#endif
