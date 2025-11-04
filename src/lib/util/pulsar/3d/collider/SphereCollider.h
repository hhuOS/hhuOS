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
 */

#ifndef HHUOS_SPHERECOLLIDER_H
#define HHUOS_SPHERECOLLIDER_H

#include "lib/util/pulsar/Collider.h"

namespace Util {
namespace Math {
template <typename T> class Vector3;
}  // namespace Math
}  // namespace Util

namespace Util::Pulsar::D3 {

class SphereCollider : public Collider {

public:
    /**
     * Constructor.
     */
    SphereCollider(const Math::Vector3<double> &position, double radius);

    /**
     * Copy Constructor.
     */
    SphereCollider(const SphereCollider &other) = default;

    /**
     * Assignment operator.
     */
    SphereCollider &operator=(const SphereCollider &other) = default;

    /**
     * Destructor.
     */
    ~SphereCollider() = default;

    [[nodiscard]] bool isColliding(const SphereCollider &other) const;

    [[nodiscard]] double getRadius() const;

private:

    double radius;
};

}

#endif
