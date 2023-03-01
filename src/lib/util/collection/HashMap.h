/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef __HashMap_include__
#define __HashMap_include__

#include "HashNode.h"
#include "Array.h"
#include "ArrayList.h"
#include "Map.h"
#include "Pair.h"

#include <cstdint>

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

    HashMap(std::initializer_list<Pair<K, V>> list);

    HashMap(const HashMap<K, V> &other) = delete;

    HashMap<K, V> &operator=(const HashMap<K, V> &other) = delete;

    ~HashMap();

    void put(const K &key, const V &value) override;

    [[nodiscard]] V get(const K &key) const override;

    V remove(const K &key) override;

    [[nodiscard]] bool containsKey(const K &key) const override;

    [[nodiscard]] uint32_t size() const override;

    void clear() override;

    [[nodiscard]] Array<K> keys() const override;

    [[nodiscard]] Array<V> values() const override;

private:

    void initialize() const;

    mutable HashNode <K, V> **table;
    mutable bool isInitialized = false;
    const uint32_t tableSize;
    uint32_t count;

    static const constexpr uint32_t DEFAULT_TABLE_SIZE = 47;
};

template<class K, class V>
HashMap<K, V>::HashMap() noexcept : tableSize(DEFAULT_TABLE_SIZE), count(0) {}

template<class K, class V>
HashMap<K, V>::HashMap(uint32_t tableSize) noexcept : tableSize(tableSize), count(0) {}

template<class K, class V>
HashMap<K, V>::HashMap(std::initializer_list<Pair<K, V>> list) : tableSize(DEFAULT_TABLE_SIZE), count(list.size()) {
    for (auto &pair : list) {
        put(pair.first, pair.second);
    }
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

    Exception::throwException(Exception::KEY_NOT_FOUND, "HashMap: Key does not exist!");
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
        Exception::throwException(Exception::KEY_NOT_FOUND, "HashMap: Key does not exist!");
    }

    V tmp = entry->getValue();

    if (previous == nullptr) {
        table[hash] = entry->getNext();
    } else {
        previous->setNext(entry->getNext());
    }

    count--;
    delete entry;
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
    if (!isInitialized) {
        initialize();
    }

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
Array<K> HashMap<K, V>::keys() const {
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

template<typename K, typename V>
Array<V> HashMap<K, V>::values() const {
    if (!isInitialized) {
        initialize();
    }

    ArrayList<V> valueList;
    HashNode<K, V> *current;

    for (uint32_t i = 0; i < tableSize; i++) {
        current = table[i];

        while (current != nullptr) {
            valueList.add(current->getValue());
            current = current->getNext();
        }
    }

    return valueList.toArray();
}

}

#endif
