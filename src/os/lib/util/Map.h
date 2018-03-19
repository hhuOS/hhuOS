#ifndef __Map_include__
#define __Map_include__

#include "Set.h"

namespace Util {

    template<typename K, typename V>
    class Map {

    public:

        virtual void put(const K &key, const V &value) = 0;

        virtual V get(const K &key) const = 0;

        virtual V remove(const K &key) = 0;

        virtual bool containsKey(const K &key) const = 0;

        virtual uint32_t size() const = 0;

        virtual void clear() = 0;

        virtual Array<K> keySet() const = 0;

    };

}


#endif
