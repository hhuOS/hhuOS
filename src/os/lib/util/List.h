#ifndef __List_include__
#define __List_include__

#include "Collection.h"

namespace Util {

    /**
     * @author Filip Krakowski
     */
    template<typename T>
    class List : public Collection<T> {

    public:

        List() = default;

        virtual ~List() = default;

        virtual bool add(const T &element) = 0;

        virtual void add(uint32_t index, const T &element) = 0;

        virtual bool addAll(const Collection<T> &other) = 0;

        virtual T get(uint32_t index) const = 0;

        virtual void set(uint32_t index, const T &element) = 0;

        virtual bool remove(const T &element) = 0;

        virtual bool removeAll(const Collection<T> &other) = 0;

        virtual T remove(uint32_t index) = 0;

        virtual bool contains(const T &element) const = 0;

        virtual bool containsAll(const Collection<T> &other) const = 0;

        virtual uint32_t indexOf(const T &element) const = 0;

        virtual bool isEmpty() const = 0;

        virtual void clear() = 0;

        virtual Iterator<T> begin() const = 0;

        virtual Iterator<T> end() const = 0;

        virtual uint32_t size() const = 0;

        virtual Array<T> toArray() const = 0;

    protected:

        virtual void ensureCapacity(uint32_t newCapacity) = 0;

    };

}


#endif
