/*****************************************************************************
 *                                                                           *
 *                              Q U E U E                                    *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung einer einfach verketteten Liste von      *
 *                  Chain Objekten. Es wird immer am Ende eingefuegt und     *
 *                  Anfang ausgetragen.                                      *
 *                                                                           *
 * Autor:           Olaf Spinczyk, TU Dortmund                               *
 *                  Michael Schoettner, HHU, 25.8.2016                       *
 *****************************************************************************/

#ifndef __deprecated_Queue_include__
#define __deprecated_Queue_include__

#include <cstdint>

extern "C" {
    #include <lib/libc/string.h>
};


template <typename T>
class Queue {

public:

    Queue();

    Queue(const Queue &copy) = delete; // Verhindere Kopieren

    void push(T *element);

    T *pop();

    void remove(T *element);

    uint32_t count();

    bool isEmpty();

    void clear();

protected:

    T **elements;

private:

    uint32_t capacity;

    uint32_t size;

    void ensureCapacity(uint32_t newCapacity);

    static const uint32_t DEFAULT_CAPACITY = 8;

};

template <class T>
Queue<T>::Queue(){
    this->size = 0;
    this->capacity = 0;
}

template <class T>
T *Queue<T>::pop(){

    if (size == 0) {
        return nullptr;
    }

    T* ret = elements[0];

    size--;

    memcpy(&elements[0], &elements[1], size * sizeof(T*));

    return ret;
}

template <class T>
void Queue<T>::push(T *element){

    ensureCapacity(size + 1);

    elements[size] = element;

    size++;
}

template <class T>
void Queue<T>::remove(T *element){

    for (uint32_t i = 0; i < size; i++) {
        if (elements[i] == element) {
            memcpy(&elements[i], &elements[i + 1], (size - i) * sizeof(T*));
            size--;
        }
    }
}

template <class T>
uint32_t Queue<T>::count() {
    return size;
}

template <class T>
bool Queue<T>::isEmpty() {
    return size == 0;
}

template <class T>
void Queue<T>::clear() {
    this->size = 0;
}

template <class T>
void Queue<T>::ensureCapacity(uint32_t newCapacity) {

    if (capacity == 0) {
        capacity = DEFAULT_CAPACITY;
        elements = new T*[capacity];
        memset(elements, 0, capacity * sizeof(T*));
    }

    if (newCapacity <= capacity) {
        return;
    }

    int oldCapacity = capacity;


    while (capacity < newCapacity) {
        capacity *= 2;
    }

    T** tmp = elements;
    elements = new T*[capacity];
    memset(elements, 0, capacity * sizeof(T*));
    memcpy(elements, tmp, oldCapacity * sizeof(T*));
    delete tmp;
}

#endif

