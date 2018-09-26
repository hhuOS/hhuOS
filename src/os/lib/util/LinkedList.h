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

template <typename T>
class LinkedList {

struct Node {
    T *elem;
    struct Node *next;
};
    
private:

    struct Node start = { nullptr, nullptr };

    uint32_t len = 0;

public:

    void add(T *newElem);

    void remove(T *rmElem);

    uint32_t length();

    bool isEmpty();

    bool contains(T *sElem);

    int32_t indexOf(T *sElem);

    T *get(uint32_t index) const;

    LinkedList();

    ~LinkedList();
};

template <class T>
LinkedList<T>::LinkedList() {}

template <class T>
LinkedList<T>::~LinkedList() {
    Node *current, *next;

    current = &start;
    next = start.next;

    for (uint32_t i = 0; i < len; i++) {
        delete current;

        current = next;
        next = current->next;
    }
}

/**
 * Adds a new element to the list's end.
 *
 * @param newElem The new element.
 */
template <class T>
void LinkedList<T>::add(T *newElem) {
    if(len == 0) {
        start.elem = newElem;
        len++;
        return;
    }

    struct Node *newNode = new Node;
    newNode -> elem = newElem;
    newNode -> next = nullptr;

    struct Node *currentNode = &start;
    while(currentNode -> next != nullptr) {
        currentNode = currentNode -> next;
    }

    currentNode -> next = newNode;
    len++;
}

/**
 * Removes a given element from the list.
 *
 * @param rmElem The element to be removed.
 */
template <class T>
void LinkedList<T>::remove(T *rmElem) {
    if(start.elem == rmElem) {
        if(start.next == nullptr) start.elem = nullptr;
        else start = *start.next;
        len--;
        return;
    }

    struct Node *lastNode;
    struct Node *currentNode = &start;
    while(currentNode -> elem != rmElem) {
        if(currentNode -> next == nullptr) return;
        lastNode = currentNode;
        currentNode = currentNode -> next;
    }

    lastNode -> next = currentNode -> next;
    len--;
}

template <class T>
uint32_t LinkedList<T>::length() {
    return len;
}

template <class T>
bool LinkedList<T>::isEmpty() {
    return len == 0;
}

/**
 * Checks, if the list contains a given element.
 *
 * @param sElem The element to be checked.
 *
 * @return true, if the list contains the element;
 *         false, if not.
 */
template <class T>
bool LinkedList<T>::contains(T *sElem) {
    struct Node currentNode = start;
    while(currentNode.elem != sElem) {
        if(currentNode.next == nullptr) return false;
        currentNode = *currentNode.next;
    }

    return true;
}

/**
 * Returns the index of a given element.
 *
 * @param sElem The element to be searched.
 *
 * @return The element's index, if the list contains the element;
 *         -1, if not.
 */
template <class T>
int32_t LinkedList<T>::indexOf(T *sElem) {
    struct Node currentNode = start;
    uint32_t i = 0;

    while(currentNode.elem != sElem) {
        if(currentNode.next == nullptr) return -1;
        currentNode = *currentNode.next;
        i++;
    }

    return i;
}

/**
 * Returns the element at a given index.
 *
 * @param index The element's index.
 *
 * @return The element, or nullptr if the index is too high.
 */
template <class T>
T *LinkedList<T>::get(uint32_t index) const {
    if(index >= len) return nullptr;

    struct Node currentNode = start;
    for(uint32_t i = 0; i < index; i++) {
        currentNode = *currentNode.next;
    }
    return currentNode.elem;
}

#endif
