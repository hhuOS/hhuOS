#ifndef __deprecated_List_include__
#define __deprecated_List_include__

#include "ListIterator.h"
#include "Collection.h"

template <typename T>
class List : public Collection<T> {

public:

    List() = default;

    virtual ~List() = default;

    virtual void add(const T &element) = 0;

    virtual void add(uint32_t index, T &element) = 0;

    virtual T *get(uint32_t index) const = 0;

    virtual T *remove(uint32_t index) = 0;

    virtual void set(uint32_t index, T &element) = 0;

    virtual uint32_t indexOf(T &element) const = 0;

    virtual ListIterator<T> begin() const = 0;

    virtual ListIterator<T> end() const = 0;

protected:

    virtual void ensureCapacity(uint32_t newCapacity) = 0;

};


#endif
