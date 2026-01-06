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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 *
 * The 3D-rendering has been rewritten using OpenGL (TinyGL) during a bachelor's thesis by Kevin Weber
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-keweb100
 *
 * The 2D particle system is based on a bachelor's thesis, written by Abdulbasir Gümüs.
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-abgue101
 */

#ifndef HHUOS_LIB_PULSAR_3D_ORIENTATION_H
#define HHUOS_LIB_PULSAR_3D_ORIENTATION_H

#include "util/math/Vector3.h"

namespace Pulsar {
namespace D3 {

/// Represents the orientation of an object in 3D space,
/// including its rotation angles around the x, y, and z axes,
/// as well as its up, right, and front direction vectors.
class Orientation {

public:
    /// Create a new orientation instance with default rotation (0, 0, 0)
    Orientation() {
        reset();
    }

    /// Set the rotation angles around the x, y, and z axes (in degrees).
    /// The up, right, and front direction vectors are updated accordingly.
    void setRotation(const Util::Math::Vector3<float> &angle);

    /// Rotate the orientation by the given angles around the x, y, and z axes (in degrees).
    /// The up, right, and front direction vectors are updated accordingly.
    void rotate(const Util::Math::Vector3<float> &angle) {
        setRotation(rotation + angle);
    }

    /// Reset the orientation to the default rotation (0, 0, 0).
    void reset() {
        setRotation(Util::Math::Vector3<float>(0, 0, 0));
    }

    /// Get the current rotation angles around the x, y, and z axes (in degrees).
    const Util::Math::Vector3<float>& getRotation() const {
        return rotation;
    }

    /// Get the up direction vector of the orientation.
    const Util::Math::Vector3<float>& getUp() const {
        return up;
    }

    /// Get the right direction vector of the orientation.
    const Util::Math::Vector3<float>& getRight() const {
        return right;
    }

    /// Get the front direction vector of the orientation.
    const Util::Math::Vector3<float>& getFront() const {
        return front;
    }

    /// Set the front direction vector of the orientation.
    /// The rotation angles and other direction vectors are updated accordingly.
    void setFront(const Util::Math::Vector3<float> &front);

    /// The world up vector, representing the global upward direction in 3D space (always {0, 1, 0}).
    static const Util::Math::Vector3<float> WORLD_UP;

private:

    Util::Math::Vector3<float> rotation;
    Util::Math::Vector3<float> up;
    Util::Math::Vector3<float> right;
    Util::Math::Vector3<float> front;
};

}
}

#endif
