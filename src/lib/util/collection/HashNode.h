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

#ifndef __HashNode_include__
#define __HashNode_include__

namespace Util {

/**
 * A structure used for storing key-value pairs inside a HashMap.
 *
 * @author Filip Krakowski
 */
template <typename K, typename V>
class HashNode {
public:

    HashNode(const K &key, const V &value);

    HashNode(const HashNode<K, V> &other) = delete;

    HashNode<K, V> &operator=(const HashNode<K, V> &other) = delete;

    ~HashNode() = default;

    [[nodiscard]] K getKey() const;

    [[nodiscard]] V getValue() const;

    void setValue(const V &value);

    [[nodiscard]] HashNode *getNext() const;

    void setNext(HashNode *next);

private:

    K key;
    V value;
    HashNode *next;
};

template <class K, class V>
K HashNode<K,V>::getKey() const {
    return key;
}

template <class K, class V>
V HashNode<K,V>::getValue() const {
    return value;
}

template <class K, class V>
void HashNode<K,V>::setValue(const V &value) {
    this->value = value;
}

template <class K, class V>
HashNode<K,V> *HashNode<K,V>::getNext() const {
    return next;
}

template <class K, class V>
void HashNode<K,V>::setNext(HashNode *next) {
    this->next = next;
}

template <class K, class V>
HashNode<K,V>::HashNode(const K &key, const V &value) : key(key), value(value) {
    next = nullptr;
}

}


#endif
