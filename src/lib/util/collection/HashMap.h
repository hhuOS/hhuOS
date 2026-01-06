/*
 * Copyright (C) 2017-2026 Heinrich Heine University Düsseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Main developers: Christian Gesse <christian.gesse@hhu.de>, Fabian Ruhland <ruhland@hhu.de>
 * Original development team: Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schöttner
 * This project has been supported by several students.
 * A full list of integrated student theses can be found here: https://github.com/hhuOS/hhuOS/wiki/Student-theses
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

#ifndef HHUOS_LIB_UTIL_HASHMAP_H
#define HHUOS_LIB_UTIL_HASHMAP_H

#include <stddef.h>

#include "util/collection/Array.h"
#include "util/collection/Map.h"

namespace Util {

/// A map implementation based on a hash table with separate chaining for collision resolution.
/// The map uses a fixed-size array to store the hash nodes, which are linked lists of key-value pairs.
/// On a hash collision, the new key-value pair is added to the linked list in the corresponding bucket.
/// Hash values are calculated by casting the key to a size_t and taking the modulo with the table size.
/// Complex data structures can be used as keys, as long as they implement the `operator size_t()`.
///
/// ## Example
/// ```c++
/// auto map = Util::HashMap<Util::String, int>();
/// map.put("One", 1); // Add a key-value pair -> {"One": 1}
/// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
/// map.put("Three", 3); // Add a key-value pair -> {"One": 1, "Two": 2, "Three": 3}
///
/// const auto value = map.get("One"); // Get the value for key "One" -> value = 1
///
/// map.remove("Two"); // Remove the key-value pair with key "Two" -> {"One": 1, "Three": 3}
/// const auto containsTwo = map.containsKey("Two"); // Check if the key "Two" exists -> containsTwo = false
///
/// for (const auto &key : map.keys()) {
///      // Prints: One: 1, Three: 3
///     Util::System::out << key << ": " << map.get(key) << Util::Io::PrintStream::ln;
/// }
///
/// Util::System::out << Util::Io::PrintStream::flush;
/// ```
template<typename K, typename V>
class HashMap final : public Map<K, V> {

public:
    /// Create a new empty HashMap instance with a default table size of 128.
    HashMap();

    /// Create a new HashMap instance with the given table size.
    explicit HashMap(size_t tableSize);

    /// HashMap is not copyable, so the copy constructor is deleted.
    HashMap(const HashMap &other) = delete;

    /// HashMap is not assignable, so the assignment operator is deleted.
    HashMap& operator=(const HashMap &other) = delete;

    /// Delete the HashMap instance and free the allocated memory.
    ~HashMap() override;

    /// Put a key-value pair into the map by copying the key and value.
    /// If the key already exists, the value is updated.
    ///
    /// ### Example
    /// ```c++
    /// auto map = Util::HashMap<Util::String, int>();
    /// map.put("One", 1); // Add a key-value pair -> {"One": 1}
    /// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
    /// map.put("One", 3); // Update the value for key "One" -> {"One": 3, "Two": 2}
    /// ```
    void put(const K &key, const V &value) override;

    /// Get the value associated with the key by copying it.
    /// If the key does not exist, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// auto map = Util::HashMap<Util::String, int>();
    /// map.put("One", 1); // Add a key-value pair -> {"One": 1}
    /// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
    ///
    /// auto value = map.get("One"); // Get the value for key "One" -> value = 1
    /// value = map.get("Two"); // Get the value for key "Two" -> value = 2
    /// value = map.get("Three"); // Panic! (key "Three" does not exist)
    /// ```
    V get(const K &key) const override;

    /// Remove the key-value pair associated with the key and return the value by copying it.
    /// If the key does not exist, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// auto map = Util::HashMap<Util::String, int>();
    /// map.put("One", 1); // Add a key-value pair -> {"One": 1}
    /// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
    /// map.put("Three", 3); // Add a key-value pair -> {"One": 1, "Two": 2, "Three": 3}
    ///
    /// auto value = map.remove("Two"); // Remove the key-value pair with key "Two" -> value = 2
    /// value = map.get("Two"); // Panic! (key "Two" does not exist anymore)
    /// ```
    V remove(const K &key) override;

    /// Check if the map contains the given key.
    /// This method should be used to avoid panics when trying to get or remove a key that does not exist.
    ///
    /// ### Example
    /// ```c++
    /// auto map = Util::HashMap<Util::String, int>();
    /// map.put("One", 1); // Add a key-value pair -> {"One": 1}
    /// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
    /// map.put("Three", 3); // Add a key-value pair -> {"One": 1, "Two": 2, "Three": 3}
    ///
    /// const auto containsOne = map.containsKey("One"); // Check if the key "One" exists -> containsOne = true
    /// const auto containsFour = map.containsKey("Four"); // Check if the key "Four" exists -> containsFour = false
    ///
    /// if (containsOne) {
    ///     Util::System::out << "Key 'One' exists with value: " << map.get("One")
    ///     << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush; // Prints: Key 'One' exists with value: 1
    /// }
    /// ```
    bool containsKey(const K &key) const override;

    /// Get the number of key-value pairs in the map.
    ///
    /// ### Example
    /// ```c++
    /// auto map = Util::HashMap<Util::String, int>();
    /// map.put("One", 1); // Add a key-value pair -> {"One": 1}
    /// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
    /// map.put("Three", 3); // Add a key-value pair -> {"One": 1, "Two": 2, "Three": 3}
    /// auto size = map.size(); // Get the number of key-value pairs -> size = 3
    ///
    /// map.remove("Two"); // Remove the key-value pair with key "Two" -> {"One": 1, "Three": 3}
    /// map.remove("Three"); // Remove the key-value pair with key "Three" -> {"One": 1}
    /// size = map.size(); // Get the number of key-value pairs -> size = 1
    /// ```
    size_t size() const override;

    /// Clear the map, removing all key-value pairs.
    ///
    /// ### Example
    /// ```c++
    /// auto map = Util::HashMap<Util::String, int>();
    /// map.put("One", 1); // Add a key-value pair -> {"One": 1}
    /// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
    /// map.put("Three", 3); // Add a key-value pair -> {"One": 1, "Two": 2, "Three": 3}
    /// auto size = map.size(); // Get the number of key-value pairs -> size = 3
    ///
    /// map.clear(); // Clear the map, removing all key-value pairs
    /// size = map.size(); // Get the number of key-value pairs -> size = 0
    /// ```
    void clear() override;

    /// Get an array of all keys in the map.
    /// The keys are copied into the array, so modifying the array does not affect the map.
    ///
    /// ### Example
    /// ```c++
    /// auto map = Util::HashMap<Util::String, int>();
    /// map.put("One", 1); // Add a key-value pair -> {"One": 1}
    /// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
    /// map.put("Three", 3); // Add a key-value pair -> {"One": 1, "Two": 2, "Three": 3}
    ///
    ///
    /// for (const auto &key : map.keys()) {
    ///      // Prints: One: 1, Two: 2, Three: 3
    ///     Util::System::out << key << ": " << map.get(key) << Util::Io::PrintStream::ln;
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::flush;
    /// ```
    Array<K> getKeys() const override;

    /// Get an array of all values in the map.
    /// The values are copied into the array, so modifying the array does not affect the map.
    ///
    /// ### Example
    /// ```c++
    /// auto map = Util::HashMap<Util::String, int>();
    /// map.put("One", 1); // Add a key-value pair -> {"One": 1}
    /// map.put("Two", 2); // Add a key-value pair -> {"One": 1, "Two": 2}
    /// map.put("Three", 3); // Add a key-value pair -> {"One": 1, "Two": 2, "Three": 3}
    ///
    /// for (const auto &value : map.values()) {
    ///     Util::System::out << value << ", "; // Prints: 1, 2, 3
    /// }
    ///
    /// Util::System::out << Util::Io::PrintStream::ln << Util::Io::PrintStream::flush;
    /// ```
    Array<V> getValues() const override;

private:

    struct HashNode {
        K key;
        V value;
        HashNode *next;
    };

    Array<HashNode*> table;
    size_t count = 0;
};

template<class K, class V>
HashMap<K, V>::HashMap() : HashMap(128) {}

template<class K, class V>
HashMap<K, V>::HashMap(const size_t tableSize) : table(tableSize) {
    for (size_t i = 0; i < tableSize; i++) {
        table[i] = nullptr;
    }
}

template<class K, class V>
HashMap<K, V>::~HashMap() {
    clear();
}

template<class K, class V>
void HashMap<K, V>::put(const K &key, const V &value) {
    // Calculate hash value of the key and find the corresponding bucket in the hash table.
    const auto hash = size_t(key) % table.length();
    HashNode *previous = nullptr;
    HashNode *entry = table[hash];

    // Traverse the linked list in the bucket to find the key or the end of the list.
    while (entry != nullptr && entry->key != key) {
        previous = entry;
        entry = entry->next;
    }

    if (entry == nullptr) {
        // If the key does not exist, create a new HashNode and add it to the linked list.
        entry = new HashNode{key, value, nullptr};

        if (previous == nullptr) {
            table[hash] = entry;
        } else {
            previous->next = entry;
        }

        count++;
    } else {
        // If the key exists, update the value of the existing HashNode.
        entry->value = value;
    }
}

template<class K, class V>
V HashMap<K, V>::get(const K &key) const {
    // Calculate hash value of the key and find the corresponding bucket in the hash table.
    const auto hash = size_t(key) % table.length();
    const auto *entry = table[hash];

    // Traverse the linked list in the bucket to find the key.
    while (entry != nullptr) {
        if (entry->key == key) {
            // If the key is found, return the value.
            return entry->value;
        }

        entry = entry->next;
    }

    // If the key does not exist, fire a panic.
    Panic::fire(Panic::KEY_NOT_FOUND, "HashMap: Trying to get a non-existent key!");
}

template<class K, class V>
V HashMap<K, V>::remove(const K &key) {
    // Calculate hash value of the key and find the corresponding bucket in the hash table.
    const auto hash = size_t(key) % table.length();
    HashNode *previous = nullptr;
    HashNode *entry = table[hash];

    // Traverse the linked list in the bucket to find the key.
    while (entry != nullptr && entry->key != key) {
        previous = entry;
        entry = entry->next;
    }

    if (entry == nullptr) {
        // If the key does not exist, fire a panic.
        Panic::fire(Panic::KEY_NOT_FOUND, "HashMap: Trying to remove a non-existent key!");
    }

    const auto value = entry->value;

    // If the key is found, remove the HashNode from the linked list.
    if (previous == nullptr) {
        table[hash] = entry->next;
    } else {
        previous->next = entry->next;
    }

    // Delete the HashNode and return the value.
    count--;
    delete entry;
    return value;
}

template<class K, class V>
bool HashMap<K, V>::containsKey(const K &key) const {
    // Calculate hash value of the key and find the corresponding bucket in the hash table.
    const auto hash = size_t(key) % table.length();
    const auto *entry = table[hash];

    // Traverse the linked list in the bucket to find the key.
    while (entry != nullptr && entry->key != key) {
        entry = entry->next;
    }

    // If the key is found, return true; otherwise, return false.
    return entry != nullptr;
}

template<class K, class V>
size_t HashMap<K, V>::size() const {
    return count;
}

template<class K, class V>
void HashMap<K, V>::clear() {
    // Iterate through each bucket in the hash table and delete all HashNodes.
    for (size_t i = 0; i < table.length(); i++) {
        auto *current = table[i];

        // Traverse the linked list in the bucket and delete each HashNode.
        while (current != nullptr) {
            auto *next = current->next;
            delete current;
            current = next;
        }

        table[i] = nullptr;
    }

    count = 0;
}

template<class K, class V>
Array<K> HashMap<K, V>::getKeys() const {
    Array<K> keys(count);
    size_t index = 0;

    for (size_t i = 0; i < table.length(); i++) {
        const auto *current = table[i];

        while (current != nullptr) {
            keys[index++] = current->key;
            current = current->next;
        }
    }

    return keys;
}

template<typename K, typename V>
Array<V> HashMap<K, V>::getValues() const {
    Array<V> values(count);
    size_t index = 0;

    for (size_t i = 0; i < table.length(); i++) {
        const auto *current = table[i];

        while (current != nullptr) {
            values[index++] = current->value;
            current = current->next;
        }
    }

    return values;
}

}

#endif
