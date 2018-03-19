#ifndef __deprecated_HashMap_include__
#define __deprecated_HashMap_include__

#include "HashNode.h"
#include "lib/Optional.h"
#include "lib/deprecated/ArrayList.h"

#include <cstdint>

extern "C" {
    #include <lib/libc/string.h>
};


template <typename K, typename V>
class HashMap {

public:

    HashMap() noexcept;

    explicit HashMap(uint32_t tableSize) noexcept;

    ~HashMap();

    void put(const K &key, const V &value);

    Optional<V> get(const K &key);

    bool remove(const K &key);

    List<K> *keySet();

    List<V> *valueSet();

private:

    HashNode<K,V> **table;

    const uint32_t tableSize;

    bool isInitialized = false;

    void initialize();

};

template <class K, class V>
HashMap<K,V>::HashMap(uint32_t tableSize) noexcept : tableSize(tableSize) {

}

template <class K, class V>
HashMap<K,V>::HashMap() noexcept : tableSize(3){

}

template <class K, class V>
HashMap<K,V>::~HashMap() {
    delete table;
}

template <class K, class V>
void HashMap<K,V>::put(const K &key, const V &value) {

    if (!isInitialized) {
        initialize();
    }

    const uint32_t hash = (uint32_t ) key % tableSize;

    HashNode<K,V> *previous = nullptr;
    HashNode<K,V> *entry = table[hash];

    while (entry != nullptr && entry->getKey() != key) {
        previous = entry;
        entry = entry->getNext();
    }

    if (entry == nullptr) {
        entry = new HashNode<K,V>(key, value);
        if (previous == nullptr) {
            table[hash] = entry;
        } else {
            previous->setNext(entry);
        }
    } else {
        entry->setValue(value);
    }

}

template <class K, class V>
Optional<V> HashMap<K,V>::get(const K &key) {

    if (!isInitialized) {
        initialize();
    }

    uint32_t hash = (uint32_t) key % tableSize;

    HashNode<K,V> *entry = table[hash];

    while (entry != nullptr) {
        if(entry->getKey() == key) {
            return Optional<V>(entry->getValue());
        }

        entry = entry->getNext();
    }

    return Optional<V>();
}

template <class K, class V>
bool HashMap<K,V>::remove(const K &key) {

    if (!isInitialized) {
        initialize();
    }

    uint32_t hash = (uint32_t) key % tableSize;

    HashNode<K,V> *previous = nullptr;
    HashNode<K,V> *entry = table[hash];

    while (entry != nullptr && entry->getKey() != key) {
        previous = entry;
        entry = entry->getNext();
    }

    if (entry == nullptr) {
        return false;
    }

    if (previous == nullptr) {
        table[hash] = entry->getNext();
    } else {
        previous->setNext(entry->getNext());
    }

    delete entry;

    return true;
}

template <class K, class V>
List<K> *HashMap<K,V>::keySet() {
    ArrayList<K> *keyList = new ArrayList<K>();

    if (!isInitialized) {
        initialize();
    }

    for(uint32_t i = 0; i < tableSize; i++) {
        if(table[i] != nullptr) {
            HashNode<K,V> *entry = table[i];

            while (entry != nullptr) {
                keyList->add(*(new K(entry->getKey())));
                entry = entry->getNext();
            }
        }
    }

    return keyList;
}

template <class K, class V>
List<V> *HashMap<K,V>::valueSet() {
    ArrayList<V> *valueList = new ArrayList<V>();

    if (!isInitialized) {
        initialize();
    }

    for(uint32_t i = 0; i < tableSize; i++) {
        if(table[i] != nullptr) {
            HashNode<K,V> *entry = table[i];

            while (entry != nullptr) {
                valueList->add(*(new V(entry->getValue())));
                entry = entry->getNext();
            }
        }
    }

    return valueList;
}


template <class K, class V>
void HashMap<K,V>::initialize() {
    table = new HashNode<K,V>*[tableSize];

    for (uint32_t i = 0; i < tableSize; i++) {
        table[i] = nullptr;
    }

    isInitialized = true;
}

#endif
