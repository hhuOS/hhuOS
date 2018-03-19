#ifndef __deprecated_Collection_include__
#define __deprecated_Collection_include__

#include <cstdint>

template <typename T>
class Collection {

public:

    Collection() = default;

    virtual ~Collection() = default;

    virtual void add(const T &element) = 0;

    virtual bool remove(T &element) = 0;

    virtual bool contains(T &element) const = 0;

    virtual bool isEmpty() const = 0;

    virtual void clear() = 0;

    virtual uint32_t size() const = 0;

};


#endif
