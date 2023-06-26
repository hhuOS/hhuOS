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

#ifndef HHUOS_ATOMICARRAY_H
#define HHUOS_ATOMICARRAY_H

#include <cstdint>
#include "Atomic.h"

namespace Util::Async {

template<typename T>
class AtomicArray {

public:
    /**
     * Constructor.
     */
    AtomicArray(uint32_t size);

    /**
     * Copy Constructor.
     */
    AtomicArray(const AtomicArray &other) = delete;

    /**
     * Assignment operator.
     */
    AtomicArray &operator=(const AtomicArray &other) = delete;

    /**
     * Destructor.
     */
    ~AtomicArray();

    Atomic<T> access(uint32_t index);

    uint32_t getSize();

private:

    T *array;
    uint32_t size;
};

template<typename T>
AtomicArray<T>::~AtomicArray() {
    delete array;
}

template
class AtomicArray<int8_t>;

template
class AtomicArray<uint8_t>;

template
class AtomicArray<int16_t>;

template
class AtomicArray<uint16_t>;

template
class AtomicArray<int32_t>;

template
class AtomicArray<uint32_t>;

}

#endif
