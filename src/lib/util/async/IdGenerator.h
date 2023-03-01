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

#ifndef HHUOS_IDGENERATOR_H
#define HHUOS_IDGENERATOR_H

#include <cstdint>

namespace Util::Async {

template<typename T>
class IdGenerator {

public:
    /**
     * Default Constructor.
     */
    IdGenerator() = default;

    /**
     * Copy Constructor.
     */
    IdGenerator(const IdGenerator &other) = delete;

    /**
     * Assignment operator.
     */
    IdGenerator &operator=(const IdGenerator &other) = delete;

    /**
     * Destructor.
     */
    ~IdGenerator() = default;

    [[nodiscard]] T next();

private:

    T idCounter = 0;
};

template
class IdGenerator<int8_t>;

template
class IdGenerator<uint8_t>;

template
class IdGenerator<int16_t>;

template
class IdGenerator<uint16_t>;

template
class IdGenerator<int32_t>;

template
class IdGenerator<uint32_t>;

}

#endif