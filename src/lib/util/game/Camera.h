/*
 * Copyright (C) 2018-2025 Heinrich-Heine-Universitaet Duesseldorf,
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

#ifndef HHUOS_CAMERA_H
#define HHUOS_CAMERA_H

#include "3d/Orientation.h"
#include "lib/util/math/Vector3.h"

namespace Util {
namespace Math {
template <typename T> class Vector2;
}  // namespace Math
}  // namespace Util

namespace Util::Game {

class Camera {

public:
    /**
     * Default Constructor.
     */
    Camera();

    /**
     * Copy Constructor.
     */
    Camera(const Camera &other) = default;

    /**
     * Assignment operator.
     */
    Camera &operator=(const Camera &other) = default;

    /**
     * Destructor.
     */
    ~Camera() = default;

    [[nodiscard]] const Math::Vector3<double>& getPosition() const;

    [[nodiscard]] const Math::Vector3<double>& getRotation() const;

    [[nodiscard]] const Math::Vector3<double>& getFrontVector() const;

    [[nodiscard]] const Math::Vector3<double>& getRightVector() const;

    void setPosition(const Math::Vector3<double> &position);

    void setPosition(const Math::Vector2<double> &position);

    void setRotation(const Math::Vector3<double> &angle);

    void translate(const Math::Vector3<double> &translation);

    void rotate(const Math::Vector3<double> &angle);

    void reset();

private:

    Math::Vector3<double> position;
    D3::Orientation orientation;
};

}

#endif
