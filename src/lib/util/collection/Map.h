/*
 * Copyright (C) 2017-2025 Heinrich Heine University Düsseldorf,
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

#ifndef HHUOS_LIB_UTIL_MAP_H
#define HHUOS_LIB_UTIL_MAP_H

#include <stddef.h>

namespace Util {

/// Base class for a Map (key-value pair) data structure.
template<typename K, typename V>
class Map {

public:
    /// The Map base class has no state, so the default constructor is sufficient.
    Map() = default;

    /// The Map base class has no state, so the default destructor is sufficient.
    virtual ~Map() = default;

    /// Put a key-value pair into the map.
    virtual void put(const K &key, const V &value) = 0;

    /// Get the value associated with the key.
    [[nodiscard]] virtual V get(const K &key) const = 0;

    /// Remove the key-value pair associated with the key and return the value.
    virtual V remove(const K &key) = 0;

    /// Check if the map contains the specified key.
    [[nodiscard]] virtual bool containsKey(const K &key) const = 0;

    /// Get the number of key-value pairs in the map.
    [[nodiscard]] virtual size_t size() const = 0;

    /// Clear the map, removing all key-value pairs.
    virtual void clear() = 0;

    /// Get an array of all keys in the map.
    [[nodiscard]] virtual Array<K> getKeys() const = 0;

    /// Get an array of all values in the map.
    [[nodiscard]] virtual Array<V> getValues() const = 0;
};

}


#endif
