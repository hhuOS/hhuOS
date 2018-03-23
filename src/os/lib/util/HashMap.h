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

#ifndef __HashMap_include__
#define __HashMap_include__

#include "HashNode.h"
#include "lib/Optional.h"
#include "Array.h"
#include "ArrayList.h"
#include "Map.h"

#include <cstdint>

extern "C" {
    #include <lib/libc/string.h>
};

namespace Util {

    /**
     * An implementation of the Map interface utilizing a hash table.
     *
     * @author Filip Krakowski
     */
    template<typename K, typename V>
    class HashMap : public Map<K, V> {

    public:

        HashMap() noexcept;

        explicit HashMap(uint32_t tableSize) noexcept;

        HashMap(const HashMap<K, V> &other) = delete;

        HashMap<K, V> &operator=(const HashMap<K, V> &other) = delete;

        ~HashMap();

        void put(const K &key, const V &value) override;

        V get(const K &key) const override;

        V remove(const K &key) override;

        bool containsKey(const K &key) const override;

        uint32_t size() const override;

        void clear() override;

        Array<K> keySet() const override;

    private:

        mutable HashNode <K, V> **table;

        mutable bool isInitialized = false;

        const uint32_t tableSize;

        void initialize() const;

        static const uint32_t DEFAULT_TABLE_SIZE = 47;

        uint32_t count;

    };

    template<class K, class V>
    HashMap<K, V>::HashMap(uint32_t tableSize) noexcept : tableSize(tableSize), count(0) {

    }

    template<class K, class V>
    HashMap<K, V>::HashMap() noexcept : tableSize(DEFAULT_TABLE_SIZE), count(0) {

    }

    template<class K, class V>
    HashMap<K, V>::~HashMap() {

        clear();

        delete[] table;
    }

    template<class K, class V>
    void HashMap<K, V>::put(const K &key, const V &value) {

        if (!isInitialized) {

            initialize();
        }

        const uint32_t hash = (uint32_t) key % tableSize;

        HashNode <K, V> *previous = nullptr;

        HashNode <K, V> *entry = table[hash];

        while (entry != nullptr && entry->getKey() != key) {

            previous = entry;

            entry = entry->getNext();
        }

        if (entry == nullptr) {

            entry = new HashNode<K, V>(key, value);

            if (previous == nullptr) {

                table[hash] = entry;

            } else {

                previous->setNext(entry);
            }

            count++;

        } else {

            entry->setValue(value);
        }
    }

    template<class K, class V>
    V HashMap<K, V>::get(const K &key) const {

        if (!isInitialized) {

            initialize();
        }

        uint32_t hash = (uint32_t) key % tableSize;

        HashNode <K, V> *entry = table[hash];

        while (entry != nullptr) {

            if (entry->getKey() == key) {

                return entry->getValue();
            }

            entry = entry->getNext();
        }

        Cpu::throwException(Cpu::Exception::KEY_NOT_FOUND);

        return *(V*) nullptr;
    }

    template<class K, class V>
    V HashMap<K, V>::remove(const K &key) {

        if (!isInitialized) {

            initialize();
        }

        uint32_t hash = (uint32_t) key % tableSize;

        HashNode <K, V> *previous = nullptr;

        HashNode <K, V> *entry = table[hash];

        while (entry != nullptr && entry->getKey() != key) {

            previous = entry;

            entry = entry->getNext();
        }

        if (entry == nullptr) {

            Cpu::throwException(Cpu::Exception::KEY_NOT_FOUND);

            return *(V*) nullptr;
        }

        V tmp = entry->getValue();

        if (previous == nullptr) {

            table[hash] = entry->getNext();

        } else {

            previous->setNext(entry->getNext());
        }

        delete entry;

        count--;

        return tmp;
    }

    template<class K, class V>
    void HashMap<K, V>::initialize() const {

        table = new HashNode <K, V> *[tableSize];

        for (uint32_t i = 0; i < tableSize; i++) {
            table[i] = nullptr;
        }

        isInitialized = true;
    }

    template<class K, class V>
    bool HashMap<K, V>::containsKey(const K &key) const {

        if (!isInitialized) {

            return false;
        }

        const uint32_t hash = (uint32_t) key % tableSize;

        HashNode <K, V> *entry = table[hash];

        while (entry != nullptr && entry->getKey() != key) {
            entry = entry->getNext();
        }

        return entry != nullptr;
    }

    template<class K, class V>
    uint32_t HashMap<K, V>::size() const {

        return count;
    }

    template<class K, class V>
    void HashMap<K, V>::clear() {

        HashNode<K, V> *current;

        HashNode<K, V> *tmp;

        for (uint32_t i = 0; i < tableSize; i++) {

            current = table[i];

            while (current != nullptr) {

                tmp = current->getNext();

                delete current;

                current = tmp;
            }

            table[i] = nullptr;
        }

        count = 0;
    }

    template<class K, class V>
    Array<K> HashMap<K, V>::keySet() const {

        if (!isInitialized) {

            initialize();
        }

        ArrayList<K> keyList;

        HashNode<K, V> *current;

        for (uint32_t i = 0; i < tableSize; i++) {

            current = table[i];

            while (current != nullptr) {

                keyList.add(current->getKey());

                current = current->getNext();
            }
        }

        return keyList.toArray();
    }

}

#endif
