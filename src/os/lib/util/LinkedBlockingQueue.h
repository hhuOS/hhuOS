#ifndef HHUOS_LINKEDBLOCKINGQUEUE_H
#define HHUOS_LINKEDBLOCKINGQUEUE_H

#include "Queue.h"

namespace Util {

template <typename T>
class LinkedBlockingQueue : public Queue<T> {

public:

    LinkedBlockingQueue() = default;

    ~LinkedBlockingQueue();

    LinkedBlockingQueue(const LinkedBlockingQueue<T> &other) = delete;

    LinkedBlockingQueue<T> &operator=(const LinkedBlockingQueue<T> &other) = delete;

    void push(const T &element) override;

    T pop() override;

    bool add(const T &element) override;

    bool addAll(const Collection<T> &other) override;

    bool remove(const T &element) override;

    bool removeAll(const Collection<T> &other) override;

    bool contains(const T &element) const override;

    bool containsAll(const Collection<T> &other) const override;

    bool isEmpty() const override;

    void clear() override;

    Iterator<T> begin() const override;

    Iterator<T> end() const override;

    uint32_t size() const override;

    Array<T> toArray() const override;

protected:

    struct Node {
        T element;
        Node *next = nullptr;

        explicit Node(T element) : element(element) {};

        ~Node() {
            next = nullptr;
        }
    };

    Node *head = nullptr;
    Node *tail = nullptr;

    uint32_t length = 0;

};

template<typename T>
LinkedBlockingQueue<T>::~LinkedBlockingQueue() {
    clear();
}

template<class T>
void LinkedBlockingQueue<T>::push(const T &element) {
    Node *node = new Node(element);

    if(head == nullptr) {
        head = node;
        tail = node;
    } else {
        tail->next = node;
        tail = node;
    }

    length++;
}

template<class T>
T LinkedBlockingQueue<T>::pop() {
    while(isEmpty()) {}

    Node *tmp = head;

    head = head->next;

    T ret = tmp->element;

    delete tmp;

    length--;

    return ret;
}

template<class T>
bool LinkedBlockingQueue<T>::add(const T &element) {
    push(element);

    return true;
}

template<class T>
bool LinkedBlockingQueue<T>::addAll(const Collection<T> &other) {
    for(const T &element : other) {
        push(element);
    }

    return true;
}

template<class T>
bool LinkedBlockingQueue<T>::remove(const T &element) {
    if (head == nullptr){
        return false;
    }

    if (head->element == element) {
        pop();

        return true;
    }

    Node *current = head;

    while(current != nullptr) {
        if(current->next->element == element) {
            Node *tmp = current->next;

            current->next = current->next->next;

            delete tmp;

            length--;

            return true;
        }
    }

    return false;
}

template<class T>
bool LinkedBlockingQueue<T>::removeAll(const Collection<T> &other) {
    bool ret = true;

    for(const T &element : other) {
        if(!remove(element)) {
            ret = false;
        }
    }

    return ret;
}

template<class T>
bool LinkedBlockingQueue<T>::contains(const T &element) const {
    Node *current = head;

    while (current->element != element) {
        if (current->next == nullptr) {
            return false;
        }

        current = current->next;
    }

    return true;
}

template<class T>
bool LinkedBlockingQueue<T>::containsAll(const Collection<T> &other) const {
    for (const T &element : other) {
        if (!contains(element)) {
            return false;
        }
    }

    return true;
}

template<class T>
bool LinkedBlockingQueue<T>::isEmpty() const {
    return length == 0;
}

template<class T>
void LinkedBlockingQueue<T>::clear() {
    while(!isEmpty()) {
        pop();
    }
}

template<class T>
Iterator<T> LinkedBlockingQueue<T>::begin() const {
    return Iterator<T>(toArray(), 0);
}

template<class T>
Iterator<T> LinkedBlockingQueue<T>::end() const {
    return Iterator<T>(toArray(), length);
}

template<class T>
uint32_t LinkedBlockingQueue<T>::size() const {
    return length;
}

template<class T>
Array<T> LinkedBlockingQueue<T>::toArray() const {
    Array<T> ret(length);

    Node *current = head;
    uint32_t i = 0;

    while(current != nullptr) {
        ret[i] = current->element;
        current = current->next;
        i++;
    }

    return ret;
}

}

#endif
