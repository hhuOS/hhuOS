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

#include "AtomicArray.h"

#include "lib/util/async/Atomic.h"

namespace Util::Async {

template<typename T>
AtomicArray<T>::AtomicArray(uint32_t size) : array(new T[size]), size(size) {}

template<typename T>
Atomic<T> AtomicArray<T>::access(uint32_t index) {
    return Atomic<T>(array[index]);
}

template<typename T>
uint32_t AtomicArray<T>::getSize() {
    return size;
}

}