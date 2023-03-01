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

#ifndef __List_include__
#define __List_include__

#include "Collection.h"

namespace Util {

/**
 * Base interface for all lists.
 *
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

    [[nodiscard]] virtual T get(uint32_t index) const = 0;

    virtual void set(uint32_t index, const T &element) = 0;

    virtual bool remove(const T &element) = 0;

    virtual bool removeAll(const Collection<T> &other) = 0;

    virtual T removeIndex(uint32_t index) = 0;

    [[nodiscard]] virtual bool contains(const T &element) const = 0;

    [[nodiscard]] virtual bool containsAll(const Collection<T> &other) const = 0;

    [[nodiscard]] virtual uint32_t indexOf(const T &element) const = 0;

    [[nodiscard]] virtual bool isEmpty() const = 0;

    virtual void clear() = 0;

    virtual Iterator<T> begin() const = 0;

    virtual Iterator<T> end() const = 0;

    [[nodiscard]] virtual uint32_t size() const = 0;

    [[nodiscard]] virtual Array<T> toArray() const = 0;

protected:

    virtual void ensureCapacity(uint32_t newCapacity) = 0;
};

}


#endif
