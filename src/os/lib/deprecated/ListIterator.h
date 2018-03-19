#ifndef __deprecated_ListIterator_include__
#define __deprecated_ListIterator_include__

#include "lib/deprecated/List.h"
#include <cstdint>

template <typename T>
class List;

template <typename T>
class ListIterator {

public:

    explicit ListIterator(const List<T> &list, uint32_t index);

    bool operator!=(const ListIterator<T> &other);

    const ListIterator<T> &operator++();

    T &operator*();

private:

    const List<T> &list;
    uint32_t index;
};

template <class T>
ListIterator<T>::ListIterator(const List<T> &list, uint32_t index) : list(list), index(index) {

}

template <class T>
bool ListIterator<T>::operator!=(const ListIterator<T> &other) {
    return this->index != other.index;
}

template <class T>
const ListIterator<T> &ListIterator<T>::operator++() {
    this->index++;
    return *this;
}

template <class T>
T &ListIterator<T>::operator*() {
    return *list.get(index);
}


#endif
