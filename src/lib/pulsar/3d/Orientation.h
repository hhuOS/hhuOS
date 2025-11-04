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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 */

#ifndef ORIENTATION_H
#define ORIENTATION_H

#include "lib/util/math/Vector3.h"

namespace Pulsar::D3 {

class Orientation {

public:
    /**
     * Default Constructor.
     */
    Orientation();

    /**
     * Copy Constructor.
     */
    Orientation(const Orientation &other) = default;

    /**
     * Assignment operator.
     */
    Orientation &operator=(const Orientation &other) = default;

    /**
     * Destructor.
     */
    ~Orientation() = default;

    void setRotation(const Util::Math::Vector3<double> &angle);

    void rotate(const Util::Math::Vector3<double> &angle);

    void reset();

    [[nodiscard]] const Util::Math::Vector3<double>& getRotation() const;

    [[nodiscard]] const Util::Math::Vector3<double>& getUp() const;

    [[nodiscard]] const Util::Math::Vector3<double>& getRight() const;

    [[nodiscard]] const Util::Math::Vector3<double>& getFront() const;

    void setFront(const Util::Math::Vector3<double> &front);

    static const Util::Math::Vector3<double> WORLD_UP;

private:

    Util::Math::Vector3<double> rotation;
    Util::Math::Vector3<double> up;
    Util::Math::Vector3<double> right;
    Util::Math::Vector3<double> front;
};

}

#endif
