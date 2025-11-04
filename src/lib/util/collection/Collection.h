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

#ifndef HHUOS_LIB_UTIL_COLLECTION_H
#define HHUOS_LIB_UTIL_COLLECTION_H

#include <stddef.h>

#include "util/collection/Iterator.h"
#include "util/collection/Array.h"

namespace Util {

/// Base class for all collections.
template <typename T>
class Collection : public Iterable<T> {

public:
    /// The Collection base class has no state, so the default constructor is sufficient.
    Collection() = default;

    /// The Collection base class has no state, so the default destructor is sufficient.
    ~Collection() override = default;

    /// Add an element to the collection by copying it.
    virtual void add(const T &element) = 0;

    /// Add all elements from the given collection by copying them.
    virtual void addAll(const Collection &collection) = 0;

    /// Remove the first occurrence of the specified element from the collection.
    virtual bool remove(const T &element) = 0;

    /// Remove all occurrences of the given collection from this collection.
    virtual bool removeAll(const Collection &collection) = 0;

    /// Retain only the elements that are also contained in the given collection.
    virtual bool retainAll(const Collection &collection) = 0;

    /// Check if the collection contains the specified element.
    [[nodiscard]] virtual bool contains(const T &element) const = 0;

    /// Check if this collection contains all elements from the given collection.
    [[nodiscard]] virtual bool containsAll(const Collection &collection) const = 0;

    /// Check if the collection is empty.
    [[nodiscard]] virtual bool isEmpty() const = 0;

    /// Clear the collection, removing all elements.
    virtual void clear() = 0;

    /// Get the number of elements in the collection.
    [[nodiscard]] virtual size_t size() const = 0;

    /// Convert the collection to an array by copying its elements.
    [[nodiscard]] virtual Array<T> toArray() const = 0;
};

}


#endif
