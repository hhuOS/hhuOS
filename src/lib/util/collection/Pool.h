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

#ifndef HHUOS_LIB_UTIL_POOL_H
#define HHUOS_LIB_UTIL_POOL_H

#include "async/AtomicBitmap.h"
#include "collection/Array.h"

#include "Pair.h"

namespace Util {

/// A simple pool implementation that allows for efficient allocation and deallocation of objects of type T.
/// The pool has a fixed size and must be filled manually.
/// It uses an array to store pointers to the objects and two atomic bitmaps to track allocated and written objects,
/// making it thread-safe.
///
/// ## Example
/// ```c++
/// // Initialize a pool for objects of type Util::Pair<int, int> with a capacity of 3.
/// auto pool = Util::Pool<Util::Pair<int, int>>(3);
/// pool.push(new Util::Pair<int, int>(1, 2));
/// pool.push(new Util::Pair<int, int>(1, 2));
/// pool.push(new Util::Pair<int, int>(1, 2));
///
/// // Retrieve an object from the pool.
/// auto *pair = pool.pop();
///
/// // Do something with the object.
/// ...
///
/// // Return the object to the pool for reuse.
/// pool.push(pair);
/// ```
template<typename T>
class Pool {
public:
    /// Create a new empty Pool instance with the given size.
    explicit Pool(size_t size);

    /// Pool is not copyable, since it contains pointers to objects.
    Pool(const Pool &copy) = delete;

    /// Pool is not copyable, since it contains pointers to objects.
    Pool& operator= (const Pool &other) = delete;

    /// Delete the Pool instance and free the array of pointers.
    /// CAUTION: This does not delete the objects pointed to by the pointers in the pool.
    ///          The caller is responsible for managing the lifetime of the objects.
    ~Pool() = default;

    /// Add an element to the pool.
    /// If the pool is full, the element is not added and the function returns false.
    /// Otherwise, the element is added to the pool and the function returns true.
    ///
    /// ### Example
    /// ```c++
    /// auto pool = Util::Pool<Util::Pair<int, int>>(3);
    /// auto success = pool.push(new Util::Pair<int, int>(1, 2)); // true
    /// success = pool.push(new Util::Pair<int, int>(1, 2)); // true
    /// success = pool.push(new Util::Pair<int, int>(1, 2)); // true
    /// success = pool.push(new Util::Pair<int, int>(1, 2)); // false (pool is full)
    /// ```
    bool push(T *element);

    /// Remove an element from the pool and return it.
    /// If the pool is empty, a panic is fired.
    ///
    /// ### Example
    /// ```c++
    /// auto pool = Util::Pool<Util::Pair<int, int>>(3);
    /// pool.push(new Util::Pair<int, int>(1, 2));
    /// pool.push(new Util::Pair<int, int>(1, 2));
    /// pool.push(new Util::Pair<int, int>(1, 2));
    ///
    /// auto *element = pool.pop(); // Returns the first element added to the pool.
    /// element = pool.pop(); // Returns the second element added to the pool.
    /// element = pool.pop(); // Returns the third element added to the pool.
    /// element = pool.pop(); // Panic! (pool is empty)
    /// ```
    [[nodiscard]] T* pop();

    /// Get the capacity of the pool, i.e. the number of elements it can hold.
    ///
    /// ### Example
    /// ```c++
    /// auto pool = Util::Pool<Util::Pair<int, int>>(3);
    /// const auto capacity = pool.getCapacity(); // 3
    /// ```
    [[nodiscard]] size_t getCapacity() const;

    /// Get the amount of objects currently in the pool.
    ///
    /// ### Example
    /// ```c++
    /// auto pool = Util::Pool<Util::Pair<int, int>>(3);
    /// auto count = pool.getObjectCount(); // 0
    ///
    /// pool.push(new Util::Pair<int, int>(1, 2));
    /// count = pool.getObjectCount(); // 1
    ///
    /// pool.push(new Util::Pair<int, int>(1, 2));
    /// count = pool.getObjectCount(); // 2
    ///
    /// pool.push(new Util::Pair<int, int>(1, 2));
    /// count = pool.getObjectCount(); // 3
    ///
    /// auto *element = pool.pop();
    /// count = pool.getObjectCount(); // 2
    /// ```
    [[nodiscard]] size_t getObjectCount() const;

    /// Check if the pool is full, i.e. if it contains as many objects as its capacity.
    ///
    /// ### Example
    /// ```c++
    /// auto pool = Util::Pool<Util::Pair<int, int>>(3);
    /// auto isFull = pool.isFull(); // false
    ///
    /// pool.push(new Util::Pair<int, int>(1, 2));
    /// pool.push(new Util::Pair<int, int>(1, 2));
    /// pool.push(new Util::Pair<int, int>(1, 2));
    /// isFull = pool.isFull(); // true
    ///
    /// auto *element = pool.pop();
    /// isFull = pool.isFull(); // false
    /// ```
    [[nodiscard]] bool isFull() const;

private:

    Array<T*> array;
    Async::AtomicBitmap allocatedMap;
    Async::AtomicBitmap writtenMap;

};

template<typename T>
Pool<T>::Pool(size_t size) : array(size), allocatedMap(size), writtenMap(size) {}

template<typename T>
bool Pool<T>::push(T *element) {
    size_t index = allocatedMap.findAndSet();
    if (index == Async::AtomicBitmap::INVALID_INDEX) {
        return false;
    }

    array[index] = element;
    writtenMap.set(index);

    return true;
}

template<typename T>
T* Pool<T>::pop() {
    size_t index = writtenMap.findAndUnset();
    if (index == Async::AtomicBitmap::INVALID_INDEX) {
        Util::Panic::fire(Util::Panic::ILLEGAL_STATE, "Pool: Out of objects!");
    }

    T *element = array[index];
    allocatedMap.unset(index);

    return element;
}

template<typename T>
size_t Pool<T>::getCapacity() const {
    return allocatedMap.getSize();
}

template<typename T>
size_t Pool<T>::getObjectCount() const {
    size_t objects = 0;
    for (size_t i = 0; i < allocatedMap.getSize(); i++) {
        if (allocatedMap.check(i)) {
            objects++;
        }
    }

    return objects;
}

template<typename T>
bool Pool<T>::isFull() const {
    return getObjectCount() == allocatedMap.getSize();
}

}

#endif
