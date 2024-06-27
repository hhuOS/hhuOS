/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_MATRIX3X3_H
#define HHUOS_MATRIX3X3_H

namespace Util::Math {

class Matrix3x3 {

public:
    /**
     * Default Constructor.
     */
    Matrix3x3() = default;

    /**
     * Copy Constructor.
     */
    Matrix3x3(const Matrix3x3 &other) = delete;

    /**
     * Assignment operator.
     */
    Matrix3x3 &operator=(const Matrix3x3 &other) = delete;

    /**
     * Destructor.
     */
    ~Matrix3x3() = default;
};

}

#endif
