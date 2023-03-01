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

#ifndef HHUOS_MATH_H
#define HHUOS_MATH_H


#include <cstdint>

namespace Util::Math {

    void endMmx();

    uint32_t absolute(int32_t value);

    uint64_t absolute(int64_t value);

    float absolute(float value);

    double absolute(double value);

    float sine(float value);

    double sine(double value);

    float cosine(float value);

    double cosine(double value);

    double sqrt(double value);

    double pow(double value, int exponent);

}


#endif
