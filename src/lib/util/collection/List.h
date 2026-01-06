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

#ifndef HHUOS_LIB_UTIL_LIST_H
#define HHUOS_LIB_UTIL_LIST_H

#include <stddef.h>

#include "util/collection/Collection.h"

namespace Util {

/// Base class for all list implementations.
template<typename T>
class List : public Collection<T> {

public:
    /// The List base class has no state, so the default constructor is sufficient.
    List() = default;

    /// The List base class has no state, so the default destructor is sufficient.
    ~List() override = default;

    /// Add an element at the specified index by copying it.
    virtual void addIndex(size_t index, const T &element) = 0;

    /// Get the element at the specified index without removing it.
    /// The element is copied from the list and returned by value.
    virtual T get(size_t index) const = 0;

    /// Overwrite the element at the specified index with the given element by copying it.
    virtual void set(size_t index, const T &element) = 0;

    /// Remove the first occurrence of the specified element from the list and return a copy of it.
    virtual T removeIndex(size_t index) = 0;

    /// Get the index of the first occurrence of the specified element in the list.
    /// If the element is not found, `INVALID_INDEX` is returned.
    virtual size_t indexOf(const T &element) const = 0;

    /// Indicates that an element was not found in the list.
    static constexpr size_t INVALID_INDEX = SIZE_MAX;
};

}


#endif
