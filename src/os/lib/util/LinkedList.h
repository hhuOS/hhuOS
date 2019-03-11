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


namespace Util {

template<typename T>
class LinkedList : public List<T> {

    struct Node {
        T element;
        Node *next = nullptr;

        explicit Node(T element) : element(element) {};
    };

private:

    Node *head = nullptr;

    uint32_t length = 0;

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
};

template<class T>
LinkedList<T>::~LinkedList() {
    clear();
}

template<class T>
bool LinkedList<T>::add(const T &element) {
    Node *node = new Node(element);

    if (head == nullptr) {
        head = node;
        length++;

        return true;
    }

    Node *current = head;

    while (current->next != nullptr) {
        current = current->next;
    }

    current->next = node;
    length++;

    return true;
}

template<typename T>
void LinkedList<T>::add(uint32_t index, const T &element) {
    if (index > length) {
        return;
    }

    Node *node = new Node(element);

    if(head == nullptr) {
        head = node;
        length++;

        return;
    }

    Node *current = head;

    for(uint32_t i = 0; i < length; i++) {
        if(i == index) {
            node->next = current->next;
            current->next = node;

            length++;
            return;
        }

        current = current->next;
    }
}

template<typename T>
bool LinkedList<T>::addAll(const Collection <T> &other) {
    for(const T &element : other) {
        add(element);
    }

    return true;
}

template<class T>
bool LinkedList<T>::remove(const T &element) {
    if (head == nullptr) {
        return false;
    }

    if (head->element == element) {
        Node *oldHead = head;

        head = head->next;

        delete oldHead;
        length--;

        return true;
    }

    Node *last = head;
    Node *current = head;

    while (current->element != element) {
        if (current->next == nullptr) {
            return false;
        }

        last = current;
        current = current->next;
    }

    last->next = current->next;

    length--;

    delete current;

    return true;
}

template<class T>
T LinkedList<T>::remove(uint32_t index) {
    if(index == 0) {
        Node *oldHead = head;

        head = head->next;

        T ret = oldHead->element;
        delete oldHead;

        length--;

        return ret;
    }

    Node *pred = head;
    Node *current = head;

    for(uint32_t i = 0; i < length; i++) {
        if(i == index) {
            pred->next = current->next;
            length--;

            T ret = current->element;
            delete current;

            return ret;
        }

        pred = current;
        current = current->next;
    }

    return current->element;
}

template<typename T>
void LinkedList<T>::set(uint32_t index, const T &element) {
    if(index >= length) {
        return;
    }

    Node *current = head;

    for(uint32_t i = 0; i < length; i++) {
        if(i == index) {
            current->element = element;

            return;
        }

        current = current->next;
    }
}

template<typename T>
bool LinkedList<T>::removeAll(const Collection <T> &other) {
    bool changed = false;

    for (const T &element : other) {
        if (remove(element)) {
            changed = true;
        }
    }

    return changed;
}

template<class T>
uint32_t LinkedList<T>::size() const {
    return length;
}

template<class T>
bool LinkedList<T>::isEmpty() const {
    return length == 0;
}

template<class T>
bool LinkedList<T>::contains(const T &element) const {
    Node *current = head;

    while (current->element != element) {
        if (current->next == nullptr) {
            return false;
        }

        current = current->next;
    }

    return true;
}

template<typename T>
bool LinkedList<T>::containsAll(const Collection <T> &other) const {
    for (const T &element : other) {
        if (!contains(element)) {
            return false;
        }
    }

    return true;
}

template<class T>
uint32_t LinkedList<T>::indexOf(const T &element) const {
    Node *current = head;
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
    if (index >= length) {
        const char *errorMessage = String::format(
                "LinkedList: Trying to access an element at index %u, but length is %u!", index, length);

        Cpu::throwException(Cpu::Exception::OUT_OF_BOUNDS, errorMessage);
    }

    Node *current = head;

    for (uint32_t i = 0; i < index; i++) {
        current = current->next;
    }

    return current->element;
}

template<typename T>
void LinkedList<T>::clear() {
    Node *current;

    current = head;

    while (current != nullptr) {
        Node *next = current->next;

        delete current;

        current = next;
    }
}

template<typename T>
Iterator <T> LinkedList<T>::begin() const {
    return Iterator<T>(toArray(), 0);
}

template<typename T>
Iterator <T> LinkedList<T>::end() const {
    return Iterator<T>(toArray(), length);
}

template<typename T>
Array<T> LinkedList<T>::toArray() const {
    Array<T> ret(length);

    Node *current = head;
    uint32_t i = 0;

    while(current != nullptr) {
        ret[i] = current->element;
        i++;
    }

    return ret;
}

template<typename T>
void LinkedList<T>::ensureCapacity(uint32_t newCapacity) {

}

}

#endif
