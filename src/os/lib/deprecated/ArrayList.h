#ifndef __deprecated_ArrayList_include__
#define __deprecated_ArrayList_include__

#include "List.h"

extern "C" {
    #include "lib/libc/string.h"
    #include <lib/libc/stdlib.h>
};

template <typename T>
class ArrayList : public List<T> {

public:

    ArrayList();

    explicit ArrayList(uint32_t capacity);

    ArrayList(const ArrayList<T> &other) = default;

    ~ArrayList();

    T *get(uint32_t index) const override;

    void add(const T &element) override;

    void add(uint32_t index, T &element) override;

    bool remove(T &element) override;

    T *remove(uint32_t index) override;

    void clear() override;

    bool contains(T &element) const override;

    uint32_t indexOf(T &element) const override;

    uint32_t size() const override;

    bool isEmpty() const override;

    void set(uint32_t index, T &element) override;

    ListIterator<T> begin() const override;

    ListIterator<T> end() const override;

private:

    T **elements = nullptr;

    uint32_t capacity = 0;

    uint32_t length = 0;

    static const uint32_t   DEFAULT_CAPACITY    = 8;

    void ensureCapacity(uint32_t newCapacity) override;

};

template <class T>
ArrayList<T>::ArrayList() {
    this->length = 0;
    this->capacity = 0;
    this->elements = nullptr;
}

template <class T>
ArrayList<T>::ArrayList(uint32_t capacity) {
    this->length = 0;
    this->capacity = capacity;
    this->elements = new T*[capacity];
}

template <class T>
ArrayList<T>::~ArrayList() {
    delete elements;
}

template <class T>
T *ArrayList<T>::get(uint32_t index) const {

    if (index >= length) {
        return nullptr;
    }

    return elements[index];
}

template <class T>
void ArrayList<T>::add(const T &element) {

    ensureCapacity(length + 1);

    elements[length] = (T*) &element;

    length++;
}

template <class T>
void ArrayList<T>::add(uint32_t index, T &element) {

    if (index > length) {
        return;
    }

    ensureCapacity(length + 1);

    memcpy(&elements[index + 1], &elements[index], capacity - index);

    elements[index] = &element;

    length++;
}

template <class T>
bool ArrayList<T>::remove(T &element) {

    T *tmp = remove(indexOf(element));

    return tmp != nullptr;
}

template <class T>
T *ArrayList<T>::remove(uint32_t index) {

    if (index >= length) {
        return nullptr;
    }

    T *tmp = elements[index];

    uint32_t numMoved = length - index - 1;
    if (numMoved != 0) {
        memcpy(&elements[index], &elements[index + 1], numMoved * sizeof(T*));
    }

    elements[--length] = nullptr;

    return tmp;
}

template <class T>
uint32_t ArrayList<T>::indexOf(T &element) const {

    uint32_t index;

    for (index = 0; elements[index] != &element && index < capacity; index++);

    return index == capacity ? UINT32_MAX : index;
}

template <class T>
bool ArrayList<T>::contains(T &element) const {
    return indexOf(element) < capacity;
}

template <class T>
void ArrayList<T>::clear() {
    memset(elements, 0, capacity * sizeof(T*));
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

template <class T>
ListIterator<T> ArrayList<T>::begin() const {
    return ListIterator<T>(*this, 0);
}

template <class T>
ListIterator<T> ArrayList<T>::end() const {
    return ListIterator<T>(*this, length);
}

template <class T>
bool ArrayList<T>::isEmpty() const {
    return length == 0;
}

template <class T>
void ArrayList<T>::set(uint32_t index, T &element) {

    if (index >= length) {
        return;
    }

    elements[index] = &element;
}

#endif
