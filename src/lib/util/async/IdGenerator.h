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

#ifndef HHUOS_LIB_UTIL_ASYNC_IDGENERATOR_H
#define HHUOS_LIB_UTIL_ASYNC_IDGENERATOR_H

#include <stddef.h>

#include "util/async/Atomic.h"

namespace Util {
namespace Async {

/// A simple thread-safe ID generator that generates unique identifiers.
/// It works by incrementing a counter atomically.
///
/// ## Example
///
/// ```c++
/// auto idGenerator = Util::Async::IdGenerator();
/// const auto id1 = idGenerator.next(); // id1 = 0
/// const auto id2 = idGenerator.next(); // id2 = 1
/// const auto id3 = idGenerator.next(); // id3 = 2
/// ```
class IdGenerator {

public:
    /// Create a new ID generator with the initial value of 0.
    IdGenerator() = default;

    /// ID generators should not be copied, since the generated values would not be unique between the copies.
    IdGenerator(const IdGenerator &other) = delete;

    /// ID generators should not be copied, since the generated values would not be unique between the copies.
    IdGenerator& operator=(const IdGenerator &other) = delete;

    /// Generate the next unique ID.
    size_t getNextId() {
        return Atomic<size_t>(idCounter).fetchAndInc();
    }

private:

    size_t idCounter = 0;
};

}
}

#endif