/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __HashSet_include__
#define __HashSet_include__

#include "Set.h"
#include "HashMap.h"

namespace Util {

    /**
     * An implementation of the Set interface backed by a HashMap.
     *
     * @author Filip Krakowski
     */
    template <typename T>
    class HashSet : public Set<T> {

    public:

        HashSet();

        explicit HashSet(uint32_t capacity);

        HashSet(const HashSet<T> &other) = delete;

        HashSet<T> &operator=(const HashSet<T> &other) = delete;

        ~HashSet() = default;

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

    private:

        HashMap<T, bool> map;

        static const uint32_t DEFAULT_CAPACITY = 47;

    };

    template<class T>
    HashSet<T>::HashSet() : map(DEFAULT_CAPACITY) {

    }

    template<class T>
    HashSet<T>::HashSet(uint32_t capacity) : map(capacity) {

    }

    template<class T>
    bool HashSet<T>::add(const T &element) {

        if (contains(element)) {
            return false;
        }

        map.put(element, true);

        return true;
    }

    template<class T>
    bool HashSet<T>::addAll(const Collection<T> &other) {

        bool modified = false;

        for (const T &element : other) {

            if (add(element)) {

                modified = true;
            }
        }

        return modified;
    }

    template<class T>
    bool HashSet<T>::remove(const T &element) {

        if (!contains(element)) {

            return false;
        }

        map.remove(element);

        return true;
    }

    template<class T>
    bool HashSet<T>::removeAll(const Collection<T> &other) {

        bool modified = false;

        for (const T &element : other) {

            if (remove(element)) {

                modified = true;
            }
        }

        return modified;
    }

    template<class T>
    bool HashSet<T>::contains(const T &element) const {

        if (!map.containsKey(element)) {
            return false;
        }

        return map.get(element);
    }

    template<class T>
    bool HashSet<T>::containsAll(const Collection<T> &other) const {

        for (const T &element : other) {

            if (!contains(element)) {

                return false;
            }
        }

        return true;
    }

    template<class T>
    bool HashSet<T>::isEmpty() const {

        return map.size() == 0;
    }

    template<class T>
    void HashSet<T>::clear() {

        map.clear();
    }

    template<class T>
    Iterator<T> HashSet<T>::begin() const {

        return Iterator<T>(toArray(), 0);
    }

    template<class T>
    Iterator<T> HashSet<T>::end() const {

        return Iterator<T>(toArray(), size());
    }

    template<class T>
    uint32_t HashSet<T>::size() const {

        return map.size();
    }

    template<class T>
    Array<T> HashSet<T>::toArray() const {

        return map.keySet();
    }
}


#endif
