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

#ifndef HHUOS_RANDOM_H
#define HHUOS_RANDOM_H

#include <stdint.h>
#include "lib/util/time/Timestamp.h"

namespace Util::Math {

class Random {

public:
    /**
     * Constructor.
     */
    explicit Random(uint32_t seed = Util::Time::getSystemTime().toMilliseconds());

    /**
     * Copy Constructor.
     */
    Random(const Random &copy) = delete;

    /**
     * Assignment operator.
     */
    Random &operator=(const Random &other) = default;

    /**
     * Destructor.
     */
    ~Random() = default;

    double nextRandomNumber();

private:

    uint32_t seed;

    static const constexpr uint32_t FACTOR = 13121797;
    static const constexpr uint32_t ADDEND = 17021856;
};

}

#endif
