#ifndef __Collection_include__
#define __Collection_include__

#include <cstdint>
#include "Iterator.h"
#include "Array.h"

namespace Util {

    /**
     * @author Filip Krakowski
     */
    template <typename T>
    class Collection {

    public:

        virtual bool add(const T &element) = 0;

        virtual bool addAll(const Collection<T> &other) = 0;

        virtual bool remove(const T &element) = 0;

        virtual bool removeAll(const Collection<T> &other) = 0;

        virtual bool contains(const T &element) const = 0;

        virtual bool containsAll(const Collection<T> &other) const = 0;

        virtual bool isEmpty() const = 0;

        virtual void clear() = 0;

        virtual Iterator<T> begin() const = 0;

        virtual Iterator<T> end() const = 0;

        virtual uint32_t size() const = 0;

        virtual Array<T> toArray() const = 0;

    };

}


#endif
