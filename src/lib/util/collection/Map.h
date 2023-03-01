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

#ifndef __Map_include__
#define __Map_include__

namespace Util {

/**
 * Base interface for all maps.
 *
 * @author Filip Krakowski
 */
template<typename K, typename V>
class Map {

public:

    virtual void put(const K &key, const V &value) = 0;

    [[nodiscard]] virtual V get(const K &key) const = 0;

    virtual V remove(const K &key) = 0;

    [[nodiscard]] virtual bool containsKey(const K &key) const = 0;

    [[nodiscard]] virtual uint32_t size() const = 0;

    virtual void clear() = 0;

    [[nodiscard]] virtual Array<K> keys() const = 0;

    [[nodiscard]] virtual Array<V> values() const = 0;
};

}


#endif
