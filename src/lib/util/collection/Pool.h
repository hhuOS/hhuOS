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

#ifndef HHUOS_POOL_H
#define HHUOS_POOL_H

#include "lib/util/async/AtomicBitmap.h"
#include "Array.h"

namespace Util {

template <typename T>
class Pool {

public:

    explicit Pool(uint32_t size);

    Pool(const Pool &copy) = delete;

    Pool& operator= (const Pool &other) = delete;

    virtual ~Pool() = default;

    bool push(T *element);

    [[nodiscard]] T* pop();

    [[nodiscard]] uint32_t getCapacity();

    [[nodiscard]] uint32_t getFillingDegree();

    [[nodiscard]] bool isFull();

private:

    Array<T*> array;
    Async::AtomicBitmap allocatedMap;
    Async::AtomicBitmap writtenMap;

};

template<typename T>
Pool<T>::Pool(uint32_t size) : array(size), allocatedMap(size), writtenMap(size) {}

template<typename T>
bool Pool<T>::push(T *element) {
    uint32_t index = allocatedMap.findAndSet();
    if (index == Async::AtomicBitmap::INVALID_INDEX) {
        return false;
    }

    array[index] = element;
    writtenMap.set(index);

    return true;
}

template<typename T>
T* Pool<T>::pop() {
    uint32_t index = writtenMap.findAndUnset();
    if (index == Async::AtomicBitmap::INVALID_INDEX) {
        Util::Exception::throwException(Util::Exception::ILLEGAL_STATE, "Pool: Out of objects!");
    }

    T *element = array[index];
    allocatedMap.unset(index);

    return element;
}

template<typename T>
uint32_t Pool<T>::getCapacity() {
    return allocatedMap.getSize();
}

template<typename T>
uint32_t Pool<T>::getFillingDegree() {
    uint32_t degree = 0;
    for (uint32_t i = 0; i < allocatedMap.getSize(); i++) {
        if (allocatedMap.check(i, true)) {
            degree++;
        }
    }

    return degree;
}

template<typename T>
bool Pool<T>::isFull() {
    return getFillingDegree() == allocatedMap.getSize();
}

}

#endif
