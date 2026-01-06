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

#ifndef HHUOS_LIB_UTIL_QUEUE_H
#define HHUOS_LIB_UTIL_QUEUE_H

#include "util/collection/Collection.h"

namespace Util {

/// Base class for all queue implementations.
/// Queues are collections that follow the FIFO (First In, First Out) principle.
/// The first element added to the queue will be the first one to be removed.
template<typename T>
class Queue : public Collection<T> {

public:
    /// The Queue base class has no state, so the default constructor is sufficient.
    Queue() = default;

    /// The Queue base class has no state, so the default destructor is sufficient.
    ~Queue() override = default;

    /// Offer an element to the queue.
    /// If the there is space in the queue, the element is added by copying it and `true` is returned.
    /// If the queue is full and cannot accept more elements, `false` is returned.
    virtual bool offer(const T &element) = 0;

    /// Remove the first element from the queue and return a copy of it.
    /// If the queue is empty, this method will block until an element is available.
    virtual T poll() = 0;

    /// Peek at the first element in the queue and return a copy of it without removing it.
    /// If the queue is empty, this method will block until an element is available.
    virtual T peek() const = 0;
};

}

#endif