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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 */

#ifndef HHUOS_CAMERA_H
#define HHUOS_CAMERA_H

#include "lib/util/math/Vector3D.h"
#include "lib/util/math/Vector2D.h"

namespace Util::Game {

class Camera {

public:
    /**
     * Default Constructor.
     */
    Camera() = default;

    /**
     * Copy Constructor.
     */
    Camera(const Camera &other) = delete;

    /**
     * Assignment operator.
     */
    Camera &operator=(const Camera &other) = delete;

    /**
     * Destructor.
     */
    ~Camera() = default;

    [[nodiscard]] const Math::Vector3D& getPosition() const;

    [[nodiscard]] const Math::Vector3D& getRotation() const;

    void setPosition(const Math::Vector3D &position);

    void setPosition(const Math::Vector2D &position);

    void setRotation(const Math::Vector3D &rotation);

    void translate(const Math::Vector3D &translation);

    void rotate(const Math::Vector3D &angle);

private:

    Math::Vector3D position{};
    Math::Vector3D rotation{};
};

}

#endif
