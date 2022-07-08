/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_MATH_H
#define HHUOS_MATH_H


#include <cstdint>

namespace Util::Math {

class Math {

public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Math() = delete;

    /**
     * Copy Constructor.
     */
    Math(const Math &other) = delete;

    /**
     * Assignment operator.
     */
    Math &operator=(const Math &other) = delete;

    /**
     * Destructor.
     */
    ~Math() = delete;

    static uint32_t absolute(int32_t value);

    static uint64_t absolute(int64_t value);

    static float sine(float value);

    static double sine(double value);

    static float cosine(float value);

    static double cosine(double value);

};

}


#endif
