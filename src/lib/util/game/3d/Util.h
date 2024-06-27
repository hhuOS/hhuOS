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
 *
 * The game engine is based on a bachelor's thesis, written by Malte Sehmer.
 * The original source code can be found here: https://github.com/Malte2036/hhuOS
 *
 * It has been enhanced with 3D-capabilities during a bachelor's thesis by Richard Josef Schweitzer
 * The original source code can be found here: https://git.hhu.de/bsinfo/thesis/ba-risch114
 */

#ifndef HHUOS_UTIL_H
#define HHUOS_UTIL_H

#include "lib/util/math/Vector3D.h"

namespace Util {
namespace Game {
namespace D3 {
class Entity;
}  // namespace D3
}  // namespace Game
template <typename T> class ArrayList;
}  // namespace Util

namespace Util::Game::D3 {

class Util {

public:
    /**
     * Default Constructor.
     * Deleted, as this class has only static members.
     */
    Util() = delete;

    /**
     * Copy Constructor.
     */
    Util(const Util &other) = delete;

    /**
     * Assignment operator.
     */
    Util &operator=(const Util &other) = delete;

    /**
     * Destructor.
     * Deleted, as this class has only static members.
     */
    ~Util() = delete;

    static Entity* findEntityUsingRaytrace(const ArrayList<Entity*> &entities, Math::Vector3D from, Math::Vector3D direction, double length, double precision = 0.1);

    static Math::Vector3D findLookAt(const Math::Vector3D &from, const Math::Vector3D &to);

    /**
     * Calculates an orbit location of start around orbitCenter at angle location orbitAngle.
     * orbitAngle should be angles in degrees; {0, 0, 1} is the forward Direction; roll values will be ignored.
     */
    static Math::Vector3D findOrbitLocation(const Math::Vector3D &start, const Math::Vector3D &orbitCenter, const Math::Vector3D &orbitAngle);

    /**
     * Calculates an orbit location of start around orbitCenter at angle location orbitAngle.
     * orbitAngle should be angles in degrees; {0, 0, 1} is the forward Direction; roll values will be ignored.
     * orbitAngle will be interpreted as relative to the current start angle around the orbitCenter.
     */
    static Math::Vector3D findOrbitLocationRelative(const Math::Vector3D &start, const Math::Vector3D &orbitCenter, const Math::Vector3D &orbitAngleRelative);
};

}

#endif
