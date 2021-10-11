/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
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

#ifndef __Queue_include__
#define __Queue_include__

#include <cstdint>
#include "Collection.h"

namespace Util::Data {

/**
 * Base interface for all queues.
 *
 * @author Filip Krakowski
 */
template<typename T>
class Queue : Collection<T> {

public:

    virtual void push(const T &element) = 0;

    virtual T pop() = 0;

    virtual bool add(const T &element) = 0;

    virtual bool addAll(const Collection<T> &other) = 0;

    virtual bool remove(const T &element) = 0;

    virtual bool removeAll(const Collection<T> &other) = 0;

    [[nodiscard]] virtual bool contains(const T &element) const = 0;

    [[nodiscard]] virtual bool containsAll(const Collection<T> &other) const = 0;

    [[nodiscard]] virtual bool isEmpty() const = 0;

    virtual void clear() = 0;

    virtual Iterator<T> begin() const = 0;

    virtual Iterator<T> end() const = 0;

    [[nodiscard]] virtual uint32_t size() const = 0;

    [[nodiscard]] virtual Array<T> toArray() const = 0;
};
}

#endif